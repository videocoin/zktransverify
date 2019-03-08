/** @file
 *****************************************************************************

 Implementation of auxiliary functions for TinyRAM.

 See tinyram_aux.hpp .

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include <cassert>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <cstring>
#include <regex>

#include <libff/common/profiling.hpp>
#include <libff/common/utils.hpp>

#include <libsnark/relations/ram_computations/rams/tinyram/tinyram_aux.hpp>

namespace libsnark {

tinyram_instruction tinyram_default_instruction = tinyram_instruction(tinyram_opcode_ANSWER, true, 0, 0, 1);

std::map<tinyram_opcode, std::string> tinyram_opcode_names =
{
    { tinyram_opcode_AND,    "and" },
    { tinyram_opcode_OR,     "or" },
    { tinyram_opcode_XOR,    "xor" },
    { tinyram_opcode_NOT,    "not" },
    { tinyram_opcode_ADD,    "add" },
    { tinyram_opcode_SUB,    "sub" },
    { tinyram_opcode_MULL,   "mull" },
    { tinyram_opcode_UMULH,  "umulh" },
    { tinyram_opcode_SMULH,  "smulh" },
    { tinyram_opcode_UDIV,   "udiv" },
    { tinyram_opcode_UMOD,   "umod" },
    { tinyram_opcode_SHL,    "shl" },
    { tinyram_opcode_SHR,    "shr" },

    { tinyram_opcode_CMPE,   "cmpe" },
    { tinyram_opcode_CMPA,   "cmpa" },
    { tinyram_opcode_CMPAE,  "cmpae" },
    { tinyram_opcode_CMPG,   "cmpg" },
    { tinyram_opcode_CMPGE,  "cmpge" },

    { tinyram_opcode_MOV,    "mov" },
    { tinyram_opcode_CMOV,   "cmov" },
    { tinyram_opcode_JMP,    "jmp" },

    { tinyram_opcode_CJMP,   "cjmp" },
    { tinyram_opcode_CNJMP,  "cnjmp" },

    { tinyram_opcode_10111,  "opcode_10111" },
    { tinyram_opcode_11000,  "opcode_11000" },
    { tinyram_opcode_11001,  "opcode_11001" },
    { tinyram_opcode_STOREB, "store.b" },
    { tinyram_opcode_LOADB,  "load.b" },

    { tinyram_opcode_STOREW, "store.w" },
    { tinyram_opcode_LOADW,  "load.w" },
    { tinyram_opcode_READ,   "read" },
    { tinyram_opcode_ANSWER, "answer" }
};

std::map<tinyram_opcode, tinyram_opcode_args> opcode_args =
{
    { tinyram_opcode_AND,     tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_OR,      tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_XOR,     tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_NOT,     tinyram_opcode_args_des_arg2 },
    { tinyram_opcode_ADD,     tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_SUB,     tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_MULL,    tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_UMULH,   tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_SMULH,   tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_UDIV,    tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_UMOD,    tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_SHL,     tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_SHR,     tinyram_opcode_args_des_arg1_arg2 },
    { tinyram_opcode_CMPE,    tinyram_opcode_args_arg1_arg2 },
    { tinyram_opcode_CMPA,    tinyram_opcode_args_arg1_arg2 },
    { tinyram_opcode_CMPAE,   tinyram_opcode_args_arg1_arg2 },
    { tinyram_opcode_CMPG,    tinyram_opcode_args_arg1_arg2 },
    { tinyram_opcode_CMPGE,   tinyram_opcode_args_arg1_arg2 },
    { tinyram_opcode_MOV,     tinyram_opcode_args_des_arg2 },
    { tinyram_opcode_CMOV,    tinyram_opcode_args_des_arg2 },
    { tinyram_opcode_JMP,     tinyram_opcode_args_arg2 },
    { tinyram_opcode_CJMP,    tinyram_opcode_args_arg2 },
    { tinyram_opcode_CNJMP,   tinyram_opcode_args_arg2 },
    { tinyram_opcode_10111,   tinyram_opcode_args_none },
    { tinyram_opcode_11000,   tinyram_opcode_args_none },
    { tinyram_opcode_11001,   tinyram_opcode_args_none },
    { tinyram_opcode_STOREB,  tinyram_opcode_args_arg2_des },
    { tinyram_opcode_LOADB,   tinyram_opcode_args_des_arg2 },
    { tinyram_opcode_STOREW,  tinyram_opcode_args_arg2_des },
    { tinyram_opcode_LOADW,   tinyram_opcode_args_des_arg2 },
    { tinyram_opcode_READ,    tinyram_opcode_args_des_arg2 },
    { tinyram_opcode_ANSWER,  tinyram_opcode_args_arg2 }
};

std::map<std::string, tinyram_opcode> opcode_values;

void ensure_tinyram_opcode_value_map()
{
    if (opcode_values.empty())
    {
        for (auto it : tinyram_opcode_names)
        {
            opcode_values[it.second] = it.first;
        }
    }
}

std::vector<tinyram_instruction> generate_tinyram_prelude(const tinyram_architecture_params &ap)
{
    std::vector<tinyram_instruction> result;
    const size_t increment = ap.w/8;
    const size_t mem_start = 1ul<<(ap.w-1);
    result.emplace_back(tinyram_instruction(tinyram_opcode_STOREW,  true, 0, 0, 0));         // 0: store.w 0, r0
    result.emplace_back(tinyram_instruction(tinyram_opcode_MOV,     true, 0, 0, mem_start)); // 1: mov r0, 2^{W-1}
    result.emplace_back(tinyram_instruction(tinyram_opcode_READ,    true, 1, 0, 0));         // 2: read r1, 0
    result.emplace_back(tinyram_instruction(tinyram_opcode_CJMP,    true, 0, 0, 7));         // 3: cjmp 7
    result.emplace_back(tinyram_instruction(tinyram_opcode_ADD,     true, 0, 0, increment)); // 4: add r0, r0, INCREMENT
    result.emplace_back(tinyram_instruction(tinyram_opcode_STOREW, false, 1, 0, 0));         // 5: store.w r0, r1
    result.emplace_back(tinyram_instruction(tinyram_opcode_JMP,     true, 0, 0, 2));         // 6: jmp 2
    result.emplace_back(tinyram_instruction(tinyram_opcode_STOREW,  true, 0, 0, mem_start)); // 7: store.w 2^{W-1}, r0
    return result;
}

size_t tinyram_architecture_params::address_size() const
{
    return dwaddr_len();
}

size_t tinyram_architecture_params::value_size() const
{
    return 2*w;
}

size_t tinyram_architecture_params::cpu_state_size() const
{
    return k * w + 2; /* + flag + tape1_exhausted */
}

size_t tinyram_architecture_params::initial_pc_addr() const
{
    /* the initial PC address is memory units for the RAM reduction */
    const size_t initial_pc_addr = generate_tinyram_prelude(*this).size();
    return initial_pc_addr;
}

libff::bit_vector tinyram_architecture_params::initial_cpu_state() const
{
    libff::bit_vector result(this->cpu_state_size(), false);
    return result;
}

memory_contents tinyram_architecture_params::initial_memory_contents(const tinyram_program &program,
                                                                     const tinyram_input_tape &primary_input) const
{
    // remember that memory consists of 1ul<<dwaddr_len() double words (!)
    memory_contents m;

    for (size_t i = 0; i < program.instructions.size(); ++i)
    {
        m[i] = program.instructions[i].as_dword(*this);
    }

    const size_t input_addr = 1ul << (dwaddr_len() - 1);
    size_t latest_double_word = (1ull<<(w-1)) + primary_input.size(); // the first word will contain 2^{w-1} + input_size (the location where the last input word was stored)

    for (size_t i = 0; i < primary_input.size()/2 + 1; ++i)
    {
        if (2*i < primary_input.size())
        {
            latest_double_word += (primary_input[2*i] << w);
        }

        m[input_addr + i] = latest_double_word;

        if (2*i + 1 < primary_input.size())
        {
            latest_double_word = primary_input[2*i+1];
        }
    }

    return m;
}

size_t tinyram_architecture_params::opcode_width() const
{
    return libff::log2(static_cast<size_t>(tinyram_opcode_ANSWER)); /* assumption: answer is the last */
}

size_t tinyram_architecture_params::reg_arg_width() const
{
    return libff::log2(k);
}

size_t tinyram_architecture_params::instruction_padding_width() const
{
    return 2 * w - (opcode_width() + 1 + 2 * reg_arg_width() + reg_arg_or_imm_width());
}

size_t tinyram_architecture_params::reg_arg_or_imm_width() const
{
    return std::max(w, reg_arg_width());
}

size_t tinyram_architecture_params::dwaddr_len() const
{
    return w-(libff::log2(w)-2);
}

size_t tinyram_architecture_params::subaddr_len() const
{
    return libff::log2(w)-2;
}

size_t tinyram_architecture_params::bytes_in_word() const
{
    return w/8;
}

size_t tinyram_architecture_params::instr_size() const
{
    return 2*w;
}

bool tinyram_architecture_params::operator==(const tinyram_architecture_params &other) const
{
    return (this->w == other.w &&
            this->k == other.k);
}

std::ostream& operator<<(std::ostream &out, const tinyram_architecture_params &ap)
{
    out << ap.w << "\n";
    out << ap.k << "\n";
    return out;
}

std::istream& operator>>(std::istream &in, tinyram_architecture_params &ap)
{
    in >> ap.w;
    libff::consume_newline(in);
    in >> ap.k;
    libff::consume_newline(in);
    return in;
}

tinyram_instruction::tinyram_instruction(const tinyram_opcode &opcode,
                                         const bool arg2_is_imm,
                                         const size_t &desidx,
                                         const size_t &arg1idx,
                                         const size_t &arg2idx_or_imm) :
    opcode(opcode),
    arg2_is_imm(arg2_is_imm),
    desidx(desidx),
    arg1idx(arg1idx),
    arg2idx_or_imm(arg2idx_or_imm)
{
}

size_t tinyram_instruction::as_dword(const tinyram_architecture_params &ap) const
{
    size_t result = static_cast<size_t>(opcode);
    result = (result << 1) | (arg2_is_imm ? 1 : 0);
    result = (result << libff::log2(ap.k)) | desidx;
    result = (result << libff::log2(ap.k)) | arg1idx;
    result = (result << (2*ap.w - ap.opcode_width() - 1 - 2 * libff::log2(ap.k))) | arg2idx_or_imm;

    return result;
}

void tinyram_architecture_params::print() const
{
    printf("* Number of registers (k): %zu\n", k);
    printf("* Word size (w): %zu\n", w);
}

tinyram_instruction random_tinyram_instruction(const tinyram_architecture_params &ap)
{
    const tinyram_opcode opcode = (tinyram_opcode)(std::rand() % (1ul<<ap.opcode_width()));
    const bool arg2_is_imm = std::rand() & 1;
    const size_t desidx = std::rand() % (1ul<<ap.reg_arg_width());
    const size_t arg1idx = std::rand() % (1ul<<ap.reg_arg_width());
    const size_t arg2idx_or_imm = std::rand() % (1ul<<ap.reg_arg_or_imm_width());
    return tinyram_instruction(opcode, arg2_is_imm, desidx, arg1idx, arg2idx_or_imm);
}

void tinyram_program::add_instruction(const tinyram_instruction &instr)
{
    instructions.emplace_back(instr);
}

tinyram_program load_preprocessed_program(const tinyram_architecture_params &ap,
                                          std::istream &preprocessed)
{
    ensure_tinyram_opcode_value_map();

    tinyram_program program;

    libff::enter_block("Loading program");
    std::string instr, line;

    while (preprocessed >> instr)
    {
        libff::print_indent();
        size_t immflag, des, a1;
        long long int a2;
        if (preprocessed.good())
        {
            preprocessed >> immflag >> des >> a1 >> a2;
            a2 = ((1ul<<ap.w)+(a2 % (1ul<<ap.w))) % (1ul<<ap.w);
            program.add_instruction(tinyram_instruction(opcode_values[instr], immflag, des, a1, a2));
        }
    }
    libff::leave_block("Loading program");

    return program;
}

// trim from start (in place)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);

    auto pos = s.find(";");
    if (pos != std::string::npos)
    {
        s.erase(s.begin()+pos, s.end());
    }

    rtrim(s);
}

static inline std::vector<std::string> split(std::string &str, const char *delim)
{
    std::vector<std::string> result;
    char line[1024];

    strcpy(line, str.c_str());

    char *pch = strtok(line, delim);
    while (pch != nullptr)
    {
        result.emplace_back(std::string(pch));
        pch = strtok(nullptr, delim);
    }

    return result;
}

static inline size_t register_to_int(const tinyram_architecture_params& ap, std::string &r)
{
    assert(!r.empty());
    std::string value = r;
    bool is_register = r[0] == 'r';
    if (is_register)
    {
        value = value.substr(1);
    }

    size_t lvalue = std::stoul(value);
    assert(!is_register || lvalue < ap.k);
    return lvalue;
}

static inline bool is_imm(std::string &r)
{
    assert(!r.empty());
    return r[0] != 'r';
}

static inline tinyram_instruction map_tokens_to_tinyram_instruction(const tinyram_architecture_params &ap, std::vector<std::string> &tokens)
{
    assert(!tokens.empty());

    auto find_result = opcode_values.find(tokens[0]);

    assert(find_result != opcode_values.end());
    auto opcode = find_result->second;

    switch (opcode_args[opcode])
    {
        case tinyram_opcode_args_des_arg1_arg2:
            assert(tokens.size() == 4);
            return tinyram_instruction(
                    opcode,
                    is_imm(tokens[3]),
                    register_to_int(ap, tokens[1]),
                    register_to_int(ap, tokens[2]),
                    register_to_int(ap, tokens[3]));

        case tinyram_opcode_args_des_arg2:
            assert(tokens.size() == 3);
            return tinyram_instruction(
                    opcode,
                    is_imm(tokens[2]),
                    register_to_int(ap, tokens[1]),
                    0,
                    register_to_int(ap, tokens[2]));

        case tinyram_opcode_args_arg1_arg2:
            assert(tokens.size() == 3);
            return tinyram_instruction(
                    opcode,
                    is_imm(tokens[2]),
                    0,
                    register_to_int(ap, tokens[1]),
                    register_to_int(ap, tokens[2]));

        case tinyram_opcode_args_arg2:
            assert(tokens.size() == 2);
            return tinyram_instruction(
                    opcode,
                    is_imm(tokens[1]),
                    0,
                    0,
                    register_to_int(ap, tokens[1]));

        case tinyram_opcode_args_none:
            assert(tokens.size() == 1);
            return tinyram_instruction(
                    opcode,
                    0, 0, 0, 0);

        case tinyram_opcode_args_arg2_des:
            assert(tokens.size() == 3);
            return tinyram_instruction(
                    opcode,
                    is_imm(tokens[1]),
                    register_to_int(ap, tokens[2]),
                    0,
                    register_to_int(ap, tokens[1]));
        default:
            assert(false);
    }
}

static inline bool is_label(std::string &line)
{
    return line[0] == '_' && line[line.size()-1] == ':';
}

static void analyze_line(std::string &line, std::map<std::string, std::string> &labels, size_t line_index)
{
    if (is_label(line))
    {
        labels.insert({ line.substr(0, line.size()-1), std::to_string(line_index) });
        line = "";
        return;
    }
}

static void preprocess_line(std::string &line, std::map<std::string, std::string> &labels, const tinyram_architecture_params &ap)
{
    if (is_label(line))
    {
        line = "";
        return;
    }

    // replace labels to address
    std::regex label("_[a-zA-Z0-9]+");
    std::smatch m;
    if (std::regex_search(line, m, label))
    {
        for (auto x: m)
        {
            assert(labels.find(x.str()) != labels.end());
            std::string str_index = labels.at(x.str());
            line = std::regex_replace(line, label, str_index);
        }
    }

    // replace input base address iba to address
    const size_t primary_input_base_addr = (1ul << (ap.dwaddr_len()-1));
    std::regex primary_input_base_addr_r("(iba)([0-9]+)");
    if (std::regex_search(line, m, primary_input_base_addr_r))
    {
        const size_t addr = primary_input_base_addr + std::stoul(m[2].str()) * ap.bytes_in_word();
        line = std::regex_replace(line, primary_input_base_addr_r, std::to_string(addr));
        return;
    }

    // replace stack base addr to address
    const size_t memory_base_addr = primary_input_base_addr - ap.bytes_in_word();

    std::regex mem_base_addr_add_r("(mba)([0-9]+)");
    if (std::regex_search(line, m, mem_base_addr_add_r))
    {
        const size_t addr = memory_base_addr - std::stoul(m[2].str()) * ap.bytes_in_word();
        line = std::regex_replace(line, mem_base_addr_add_r, std::to_string(addr));
    }
}

tinyram_program preprocess_raw_program(const tinyram_architecture_params &ap, std::istream &raw)
{
    ensure_tinyram_opcode_value_map();

    tinyram_program program;

    libff::enter_block("Preprocessing program");
    std::string line;
    std::map<std::string, std::string> labels;
    std::stringstream ss;

    size_t line_counter = 0;
    while (std::getline(raw, line))
    {
        trim(line);
        analyze_line(line, labels, line_counter);
        if (line.empty()) continue;
        if (line.rfind(";", 0) == 0) continue;

        auto tokens = split(line, " ,\t");
        if (tokens.empty()) continue;

        line_counter++;
        ss << line << std::endl;
    }

    raw.clear();
    raw.seekg(0, std::ios::beg);

    while (std::getline(ss, line))
    {
        std::cout << line << std::endl;
        preprocess_line(line, labels, ap);
        auto tokens = split(line, " ,\t");
        program.add_instruction(map_tokens_to_tinyram_instruction(ap, tokens));
    }

    libff::leave_block("Preprocessing program");

    return program;
}

memory_store_trace tinyram_boot_trace_from_program_and_input(const tinyram_architecture_params &ap,
                                                             const size_t boot_trace_size_bound,
                                                             const tinyram_program &program,
                                                             const tinyram_input_tape &primary_input)
{
    // TODO: document the reverse order here

    memory_store_trace result;

    size_t boot_pos = boot_trace_size_bound-1;
    for (size_t i = 0; i < program.instructions.size(); ++i)
    {
        result.set_trace_entry(boot_pos--, std::make_pair(i, program.instructions[i].as_dword(ap)));
    }

    const size_t primary_input_base_addr = (1ul << (ap.dwaddr_len()-1));

    for (size_t j = 0, k = 0; j < primary_input.size(); j += 2, ++k)
    {
        const size_t memory_dword = primary_input[j] + ((j+1 < primary_input.size() ? primary_input[j+1] : 0) << ap.w);
        result.set_trace_entry(boot_pos--, std::make_pair(primary_input_base_addr + k, memory_dword));
    }

    return result;
}

memory_store_trace tinyram_boot_trace_from_program_and_input2(const tinyram_architecture_params &ap,
                                                             const size_t boot_trace_size_bound,
                                                             const tinyram_program &program,
                                                             const tinyram_input_tape &primary_input)
{
    // TODO: document the reverse order here

    memory_store_trace result;

    size_t boot_pos = boot_trace_size_bound-1;
    for (size_t i = 0; i < program.instructions.size(); ++i)
    {
        result.set_trace_entry(boot_pos--, std::make_pair(i, program.instructions[i].as_dword(ap)));
    }

    const size_t primary_input_base_addr = (1ul << (ap.dwaddr_len()-1));

    for (size_t j = 0, k = 0; j < primary_input.size(); ++k)
    {
        assert(primary_input[j] < 256);
        size_t memory_dword = 0;
        for (size_t z = 0; z < ap.w * 2 && j < primary_input.size(); z += 8, ++j)
        {
            memory_dword += primary_input[j] << z;
        }

        result.set_trace_entry(boot_pos--, std::make_pair(primary_input_base_addr + k, memory_dword));
    }

    return result;
}

tinyram_input_tape load_tape(std::istream &tape)
{
    libff::enter_block("Loading tape");
    tinyram_input_tape result;

    libff::print_indent();
    printf("Tape contents:");
    size_t cell;
    while (tape >> cell)
    {
        printf("\t%zu", cell);
        result.emplace_back(cell);
    }
    printf("\n");

    libff::leave_block("Loading tape");
    return result;
}

} // libsnark
