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

// Nested Keyvalue stuctures from vmf files
struct KvNode {
    std::string key;
    std::string value;
    bool isBlock = false;
    std::vector<KvNode> children;
};

bool tokenize(const std::string& text, std::vector<Token>& tokens);
bool parseKeyValues(const std::vector<Token>& tokens, KvNode& root);
int countBlocks(const KvNode& node, const std::string& name);