#include "ev_lexer.hpp"
#include <cctype>

std::string token_type_name(TokenType tipo) {
    switch (tipo) {
        case TokenType::Numero:        return "Numero";
        case TokenType::Identificador: return "Identificador";
        case TokenType::ParenEsq:      return "ParenEsq";
        case TokenType::ParenDir:      return "ParenDir";
        case TokenType::Soma:          return "Soma";
        case TokenType::Sub:           return "Sub";
        case TokenType::Mult:          return "Mult";
        case TokenType::Div:           return "Div";
        case TokenType::PontoVirgula:  return "PontoVirgula";
        case TokenType::Igual:         return "Igual";
    }
    return "?";
}

// true for the whitespace characters we skip between tokens (space, tab,
// newline, carriage return).
static bool is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static bool is_letra(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

static bool is_letra_digito(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

bool tokenize(const std::string& source, std::vector<Token>& tokens, LexError& error) {
    size_t i = 0;
    const size_t n = source.size();

    while (i < n) {
        char c = source[i];

        if (is_space(c)) {
            i++;
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c))) {
            size_t start = i;
            while (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                i++;
            }
            // A digit run immediately followed by a letter (e.g. "237axy")
            // is a lexical error, not a number token followed by an
            // identifier: <num> is only digits.
            if (i < n && is_letra(source[i])) {
                error = {i, source[i]};
                return false;
            }
            tokens.push_back({TokenType::Numero, source.substr(start, i - start), start});
            continue;
        }

        if (is_letra(c)) {
            size_t start = i;
            while (i < n && is_letra_digito(source[i])) {
                i++;
            }
            tokens.push_back({TokenType::Identificador, source.substr(start, i - start), start});
            continue;
        }

        TokenType tipo;
        switch (c) {
            case '(': tipo = TokenType::ParenEsq; break;
            case ')': tipo = TokenType::ParenDir; break;
            case '+': tipo = TokenType::Soma; break;
            case '-': tipo = TokenType::Sub; break;
            case '*': tipo = TokenType::Mult; break;
            case '/': tipo = TokenType::Div; break;
            case ';': tipo = TokenType::PontoVirgula; break;
            case '=': tipo = TokenType::Igual; break;
            default:
                error = {i, c};
                return false;
        }
        tokens.push_back({tipo, std::string(1, c), i});
        i++;
    }

    return true;
}
