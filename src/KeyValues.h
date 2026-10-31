#pragma once
#include <string>
#include <vector>

enum class TokenKind {
    String,
    OpenBrace,
    CloseBrace,
};

struct Token {
    TokenKind type;
    std::string text;
};

bool tokenize(const std::string& text, std::vector<Token>& tokens);