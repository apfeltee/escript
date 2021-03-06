// Operators.cpp
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "escript.h"

namespace EScript
{
    //! (static)
    Operator::operatorMap_t Operator::ops;

    //! (static)
    const Operator* Operator::getOperator(StringId id)
    {
        if(ops.empty())
        {
            int p = -1;
            declareOperator(p, "UNKNOWN");
            declareOperator(++p, "_");
            declareOperator(p, "@", R);
            declareOperator(++p, ".");
            declareOperator(p, "[");
            declareOperator(p, "]");
            declareOperator(p, "(");
            declareOperator(p, ")");

            declareOperator(++p, "new");
            declareOperator(++p, "fn", R);
            //		declareOperator(++p ,"lambda",R);

            declareOperator(++p, "++", R);
            declareOperator(p, "--", R);
            declareOperator(p, "_-", R);// 2+-1
            declareOperator(p, "~", R);
            declareOperator(p, "!", R);

            declareOperator(++p, "*");
            declareOperator(p, "/");
            declareOperator(++p, "%");
            declareOperator(++p, "+");
            declareOperator(p, "-");
            declareOperator(++p, "<<");
            declareOperator(p, ">>");
            declareOperator(++p, "<");
            declareOperator(p, ">");
            declareOperator(p, ">=");
            declareOperator(p, "<=");
            declareOperator(p, "~=");
            declareOperator(++p, "==");
            declareOperator(p, "!=");
            declareOperator(p, "===");
            declareOperator(p, "!==");
            declareOperator(p, "---|>");
            //		declareOperator(p   ,"isA");
            declareOperator(p, "->");// ??
            declareOperator(++p, "=>");
            declareOperator(++p, "&");
            declareOperator(++p, "^");
            declareOperator(++p, "|");
            declareOperator(++p, "&&", R);
            declareOperator(++p, "||", R);

            //declareOperator(++p ,":");
            declareOperator(++p, "?");//R
            declareOperator(++p, "=", R);
            declareOperator(++p, ":=", R);
            declareOperator(++p, "::=", R);
            declareOperator(p, "*=", R);
            declareOperator(p, "/=", R);
            declareOperator(p, "%=", R);
            declareOperator(p, "+=", R);
            declareOperator(p, "-=", R);
            declareOperator(p, "&=", R);
            declareOperator(p, "^=", R);
            declareOperator(p, "|=", R);
            declareOperator(++p, "...");
            declareOperator(++p, ",");
        }
        operatorMap_t::const_iterator it = ops.find(id);
        return it == ops.end() ? nullptr : &it->second;
    }

    //! (static)
    void Operator::declareOperator(int precedence, const std::string& op, associativity_t associativity)
    {
        const StringId id(op);
        ops.insert(std::make_pair(id, Operator(id, precedence, op, associativity)));
    }

    //! (ctor)
    Operator::Operator(StringId _id, int _precedence, const std::string& _s, associativity_t _associativity)
    : id(_id), precedence(_precedence), s(_s), associativity(_associativity)
    {
    }

}// namespace EScript
