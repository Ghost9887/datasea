#include <value.h>

std::string value_to_string(const Value &value) {
    return std::visit([](auto &&arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;

        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "TRUE" : "FALSE";

        } else if constexpr (std::is_arithmetic_v<T>) {
            return std::to_string(arg);

        } else if constexpr (std::is_same_v<T, std::vector<Value>>) {
            std::string out = "[";
            bool first = true;
            for (const auto &v : arg) {
                if (!first) out += ", ";
                first = false;
                out += value_to_string(v);
            }
            out += "]";
            return out;

        } else if constexpr (std::is_same_v<T, std::monostate>) {
            return "NULL";

        } else {
            static_assert(!sizeof(T), "Unhandled type in variant");
            return "NULL";
        }
    }, value.m_data);
}

