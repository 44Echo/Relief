#include "KeyValues.h"
#include <utility>

static bool isWhiteSpace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool tokenize(const std::string &text, std::vector<Token> &tokens)
{

    size_t i = 0;
    while (i < text.size())
    {
        char c = text[i];
        // Set a location where C starts and move i forwards

        // Skip whitespaces
        if (isWhiteSpace(c))
        {
            i++;
        }
        // Add OpenBrace token
        else if (c == '{')
        {
            tokens.push_back({TokenKind::OpenBrace, "{"});
            i++;
        }
        // Add CloseBrace token
        else if (c == '}')
        {
            tokens.push_back({TokenKind::CloseBrace, "}"});
            i++;
        }
        // Add Quote token
        else if (c == '"')
        {
            i++;
            size_t start = i;
            while (i < text.size() && text[i] != '"')
            {
                i++;
            }
            if (i == text.size())
            {
                return false;
            }
            tokens.push_back({TokenKind::String, text.substr(start, i - start)});
            i++;
        }
        // Add string token
        else
        {
            size_t start = i;
            while (i < text.size() && !isWhiteSpace(text[i]) && text[i] != '{' && text[i] != '}' && text[i] != '"')
            {
                i++;
            }
            tokens.push_back({TokenKind::String, text.substr(start, i - start)});
        }
    }
    return true;
}

static bool parseChildren(const std::vector<Token>& tokens, size_t& i, KvNode& parent){
    while (i < tokens.size()) {
        const Token& t = tokens[i];

        if (t.type == TokenKind::CloseBrace) {
            return true;
        }
        if (t.type != TokenKind::String) {
            return false;
        }

        std::string key = t.text;
        i++;
        if (i >= tokens.size()) {
            return false;
        }

        const Token& next = tokens[i];
        // Three possible cases
        if (next.type == TokenKind::String) {
            KvNode pair;
            pair.key = key;
            pair.value = next.text;
            parent.children.push_back(std::move(pair));
            i++;
        }
        else if (next.type == TokenKind::OpenBrace) {
            i++;
            KvNode block;
            block.key = key;
            block.isBlock = true;
            if (!parseChildren(tokens, i, block)) {
                return false;
            }
            if (i >= tokens.size() || tokens[i].type != TokenKind::CloseBrace) {
                return false;
            }
            i++; //Step past the closing brace
            parent.children.push_back(std::move(block));
        }
        else {
            return false;
        }

    }
    return true;
}

bool parseKeyValues(const std::vector<Token>& tokens, KvNode& root) {
    size_t i = 0;
    if (!parseChildren(tokens, i, root)) {
        return false;
    }
    return i == tokens.size(); //Make sure all available tokens were consumed
}


/*
Here we actually count the numbers of blocks with a given name, such as Solids or Displacements, for Relief Displacements is the critical part.
*/
int countBlocks(const KvNode& node, const std::string& name){
    int count = 0;
    if (node.isBlock && node.key == name) {
        count++;
    }
    for (const KvNode& child : node.children) {
        count += countBlocks(child, name);
    }
    return count;
}