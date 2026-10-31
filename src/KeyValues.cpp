#include "KeyValues.h"

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