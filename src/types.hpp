#pragma once

enum class Register {
    RA,
    RB,
    RC,
    RD,
    RE,
    RF,
    RG,
    RH
};

enum class BinaryOp {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD
};

enum class BinaryCondOp {
    EQ,
    NEQ,
    LT,
    LTE,
    GT,
    GTE
};
