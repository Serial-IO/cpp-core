#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
from typing import Any


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Reduce a clang AST JSON dump to compact FFI API metadata.")
    parser.add_argument("--input", required=True, help="Path to the full clang AST JSON file.")
    parser.add_argument("--output", required=True, help="Path to the reduced output JSON file.")
    parser.add_argument("--source-root", required=True, help="Repository root used for path normalization.")
    parser.add_argument("--public-header", required=True, help="Stable public header exposed to consumers.")
    return parser.parse_args()


def walk(node: Any):
    stack = [node]
    while stack:
        current = stack.pop()
        if isinstance(current, dict):
            yield current
            for value in reversed(list(current.values())):
                if isinstance(value, (dict, list)):
                    stack.append(value)
        elif isinstance(current, list):
            for item in reversed(current):
                if isinstance(item, (dict, list)):
                    stack.append(item)


def first_file(*candidates: Any) -> str | None:
    for candidate in candidates:
        if isinstance(candidate, dict):
            file_value = candidate.get("file")
            if file_value:
                return file_value
    return None


def declaration_file(node: dict[str, Any]) -> str | None:
    loc = node.get("loc", {})
    range_begin = node.get("range", {}).get("begin", {})
    range_end = node.get("range", {}).get("end", {})
    return first_file(
        range_begin.get("expansionLoc"),
        range_begin.get("spellingLoc"),
        loc,
        range_begin,
        range_end,
        loc.get("includedFrom"),
        range_begin.get("includedFrom"),
        range_end.get("includedFrom"),
    )


def normalize_path(path: str | None, source_root: Path) -> str | None:
    if not path:
        return None
    path_obj = Path(path).resolve()
    try:
        return path_obj.relative_to(source_root).as_posix()
    except ValueError:
        return path_obj.as_posix()


def normalize_header_path(path: str | None, source_root: Path) -> str | None:
    normalized = normalize_path(path, source_root)
    if normalized and normalized.startswith("include/"):
        return normalized.removeprefix("include/")
    return normalized


def has_default_visibility(node: dict[str, Any]) -> bool:
    return any(
        isinstance(child, dict)
        and child.get("kind") == "VisibilityAttr"
        and child.get("visibility") == "default"
        for child in node.get("inner", [])
    )


def is_exported_function(node: dict[str, Any], source_root: Path) -> bool:
    if node.get("kind") != "FunctionDecl" or not node.get("name"):
        return False
    if not has_default_visibility(node):
        return False

    mangled_name = node.get("mangledName")
    if mangled_name and mangled_name != node.get("name"):
        return False

    path = normalize_path(declaration_file(node), source_root)
    return bool(path and path.startswith("include/cpp_core/"))


def split_top_level(text: str) -> list[str]:
    parts: list[str] = []
    current: list[str] = []
    angle_depth = 0
    paren_depth = 0
    square_depth = 0

    for char in text:
        if char == "<":
            angle_depth += 1
        elif char == ">":
            angle_depth = max(0, angle_depth - 1)
        elif char == "(":
            paren_depth += 1
        elif char == ")":
            paren_depth = max(0, paren_depth - 1)
        elif char == "[":
            square_depth += 1
        elif char == "]":
            square_depth = max(0, square_depth - 1)

        if char == "," and angle_depth == 0 and paren_depth == 0 and square_depth == 0:
            part = "".join(current).strip()
            if part:
                parts.append(part)
            current = []
            continue

        current.append(char)

    tail = "".join(current).strip()
    if tail:
        parts.append(tail)
    return parts


def parse_function_pointer(type_name: str) -> dict[str, Any] | None:
    match = re.match(r"^(?P<return_type>.+?)\s*\(\*\)\((?P<params>.*)\)$", type_name)
    if not match:
        return None

    params_text = match.group("params").strip()
    params = [] if not params_text or params_text == "void" else split_top_level(params_text)
    return {
        "return_type": match.group("return_type").strip(),
        "parameters": params,
    }


def extract_default_expr(node: dict[str, Any] | None) -> Any:
    if not node:
        return None

    kind = node.get("kind")
    if kind in {"ImplicitCastExpr", "ConstantExpr", "ExprWithCleanups"}:
        for child in node.get("inner", []):
            if isinstance(child, dict):
                return extract_default_expr(child)
        return None
    if kind == "IntegerLiteral":
        return node.get("value")
    if kind == "FloatingLiteral":
        return node.get("value")
    if kind == "StringLiteral":
        return node.get("value")
    if kind == "CXXBoolLiteralExpr":
        return node.get("value")
    if kind == "CXXNullPtrLiteralExpr":
        return "nullptr"
    if kind == "DeclRefExpr":
        referenced = node.get("referencedDecl", {})
        return referenced.get("name") or node.get("name")
    if kind == "UnaryOperator":
        inner = next((child for child in node.get("inner", []) if isinstance(child, dict)), None)
        inner_value = extract_default_expr(inner)
        if inner_value is None:
            return None
        return f"{node.get('opcode', '')}{inner_value}"
    return None


def text_from_comment(node: dict[str, Any]) -> str:
    kind = node.get("kind")
    if kind == "TextComment":
        return node.get("text", "")
    if kind == "InlineCommandComment":
        args = node.get("args", [])
        if node.get("renderKind") == "monospaced":
            return " ".join(f"`{arg}`" for arg in args)
        return " ".join(args)

    chunks = []
    for child in node.get("inner", []):
        if isinstance(child, dict):
            text = text_from_comment(child)
            if text:
                chunks.append(text)
    joined = " ".join(chunks)
    joined = re.sub(r"\s+", " ", joined).strip()
    joined = re.sub(r"`([^`]+?)([.,;:])`", r"`\1`\2", joined)
    joined = re.sub(r"\s+([.,;])", r"\1", joined)
    return joined


def extract_comment(node: dict[str, Any]) -> dict[str, Any]:
    full_comment = next(
        (child for child in node.get("inner", []) if isinstance(child, dict) and child.get("kind") == "FullComment"),
        None,
    )
    if not full_comment:
        return {}

    doc: dict[str, Any] = {"params": {}}
    details: list[str] = []

    for child in full_comment.get("inner", []):
        if not isinstance(child, dict):
            continue

        kind = child.get("kind")
        if kind == "BlockCommandComment" and child.get("name") == "brief":
            brief = text_from_comment(child)
            if brief:
                doc["brief"] = brief
            continue

        if kind == "BlockCommandComment" and child.get("name") in {"return", "returns"}:
            returns = text_from_comment(child)
            if returns:
                doc["returns"] = returns
            continue

        if kind == "ParamCommandComment":
            name = child.get("param")
            text = text_from_comment(child)
            if name and text:
                param_entry: dict[str, Any] = {"description": text}
                direction = child.get("direction")
                if direction:
                    param_entry["direction"] = direction
                doc["params"][name] = param_entry
            continue

        if kind == "ParagraphComment":
            paragraph = text_from_comment(child)
            if paragraph:
                details.append(paragraph)

    if details:
        doc["details"] = details
    if not doc["params"]:
        doc.pop("params")
    return doc


def extract_return_type(function_type: str) -> str:
    if "->" in function_type:
        return function_type.rsplit("->", 1)[1].strip()
    match = re.match(r"^(?P<return_type>.+?)\s*\(", function_type)
    return match.group("return_type").strip() if match else function_type


def build_parameter(node: dict[str, Any], doc_params: dict[str, Any]) -> dict[str, Any]:
    type_info = node.get("type", {})
    public_type = type_info.get("qualType")
    callback_source_type = type_info.get("desugaredQualType") or public_type
    parameter: dict[str, Any] = {
        "name": node.get("name", ""),
        "type": public_type,
    }

    default_expr = next((child for child in node.get("inner", []) if isinstance(child, dict)), None)
    default_value = extract_default_expr(default_expr)
    if default_value is not None:
        parameter["default"] = default_value
        parameter["optional"] = True
    else:
        parameter["optional"] = False

    callback = parse_function_pointer(callback_source_type)
    if callback:
        parameter["callback"] = callback

    doc_entry = doc_params.get(parameter["name"])
    if doc_entry:
        parameter["doc"] = doc_entry

    return parameter


def build_function(node: dict[str, Any], source_root: Path, public_header: str) -> dict[str, Any]:
    type_name = node.get("type", {}).get("qualType", "")
    doc = extract_comment(node)
    doc_params = doc.pop("params", {})

    function: dict[str, Any] = {
        "name": node["name"],
        "symbol": node.get("mangledName", node["name"]),
        "public_header": public_header,
        "declared_in": normalize_header_path(declaration_file(node), source_root),
        "return_type": extract_return_type(type_name),
        "parameters": [
            build_parameter(child, doc_params)
            for child in node.get("inner", [])
            if isinstance(child, dict) and child.get("kind") == "ParmVarDecl"
        ],
    }

    if node.get("inline"):
        function["inline"] = True
    if doc:
        function["doc"] = doc

    return function


def main() -> int:
    args = parse_args()
    source_root = Path(args.source_root).resolve()
    input_path = Path(args.input)
    output_path = Path(args.output)

    with input_path.open(encoding="utf-8") as input_file:
        ast = json.load(input_file)

    functions = [
        build_function(node, source_root, args.public_header)
        for node in walk(ast)
        if is_exported_function(node, source_root)
    ]
    functions.sort(key=lambda item: item["name"])

    metadata = {
        "schema": "cpp_core_ffi_api",
        "schema_version": 1,
        "public_header": args.public_header,
        "functions": functions,
    }

    output_path.parent.mkdir(parents=True, exist_ok=True)
    with output_path.open("w", encoding="utf-8") as output_file:
        json.dump(metadata, output_file, indent=2)
        output_file.write("\n")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
