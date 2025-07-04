/*  SDT - Scientific Document Tool
 *  Copyright (C) 2022-2025  Valentyn Tymchyshyn
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or any
 *  later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
 *  Valentyn Tymchyshyn (val.t.develop) (val.t.develop@gmail.com)
 *
 *  Parser.
 */
#include "Parser.hpp"
#include <utils/ArgsParser.hpp>

map<string, Parser::Obj> Parser::objs = map<string, Obj>();

Parser::Parser() {
    xmlInitParser();
    xml_document = xmlReadFile(ArgsParser::src->getName().c_str(), nullptr, XML_PARSE_NOBLANKS);
    xsl_gen = XSLGen();
}

Parser::~Parser() {
    xmlCleanupParser();
}

void Parser::parse() {
    xmlNode *root = xmlDocGetRootElement(xml_document);
    genNode(root, 0);
}

void Parser::genNode(xmlNode *node, size_t parent) {
    string base = "block";
    map<string, string> attrs = map<string, string>();
    if (objs.contains(string((char*) node->name))) {
        Obj obj = objs[string((char*) node->name)];
        if (!obj.base.empty()) {
            base = obj.base;
        }
        for (auto el : obj.args) {
            attrs[el.first] = el.second;
        }
    }

    for (auto attr = node->properties; attr; attr = attr->next) {
        attrs[(char*) attr->name] = (char*) attr->children->content;
    }

    size_t id = 0;
    if (base == "block") {
        id = xsl_gen.addBlock(parent, attrs);
    }

    for (auto el = node->children; el; el = el->next) {
        if (el->type == XML_ELEMENT_NODE) {
            genNode(el, id);
        } else if (el->type == XML_TEXT_NODE) {
            xsl_gen.addText(id, (char*) el->content);
        }
    }
}
