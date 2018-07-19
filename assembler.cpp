/*  assembler.cpp
 *
 *  Created by Stavros Avramidis on 7/1/18.
*/

#include "assembler.hpp"

namespace asmbl {

Assembler::Assembler(bool mode = false) : onDebug{mode} {

  rom.reserve(SIZE);

  //Basic operationns
  opCodes["NOP"] = 0b0000000;
  opCodes["RI"] = 0b0000001;
  opCodes["LDA"] = 0b0000010;
  opCodes["LDI"] = 0b0000011;
  opCodes["STA"] = 0b0000100;
  opCodes["JMP"] = 0b0000101;
  opCodes["MI"] = 0b0000110;
  opCodes["MO"] = 0b0000111;


  //Conditional Jumps
  opCodes["JC"] = 0b0001000;
  opCodes["JO"] = 0b0001001;
  opCodes["JG"] = 0b0001010;
  opCodes["JGE"] = 0b0001011;
  opCodes["JE"] = 0b0001100;
  opCodes["JLE"] = 0b0001101;
  opCodes["JL"] = 0b0001110;
  opCodes["JNO"] = 0b0001111;
  opCodes["JZ"] = 0b0010000;
  opCodes["JNZ"] = 0b0010001;
  opCodes["JS"] = 0b0010010;
  opCodes["JNS"] = 0b0010011;

  // ALU
  opCodes["ADD"] = 0b1000000;
  opCodes["SUB"] = 0b1000001;
  opCodes["MUL"] = 0b1000010;
  opCodes["DIV"] = 0b1000011;
  opCodes["ADDI"] = 0b1000100;
  opCodes["SUBI"] = 0b1000101;
  opCodes["MULI"] = 0b1000110;
  opCodes["DIVI"] = 0b1000111;
  //Complex ALU operations
  opCodes["SQRT"] = 0b1001000;
  opCodes["MOD"] = 0b1001001;

  // Bitwise operationns
  opCodes["AND"] = 0b1100000;
  opCodes["OR"] = 0b11000001;
  opCodes["NAND"] = 0b1100010;
  opCodes["NOR"] = 0b1100011;
  opCodes["NOT"] = 0b1100100;
  opCodes["XOR"] = 0b1100101;
  opCodes["XOR"] = 0b1100110;
  opCodes["SHL"] = 0b1100111;
  opCodes["SHR"] = 0b1101000;
  opCodes["ROT"] = 0b1101001;
  opCodes["CMP"] = 0b1101010;

  //FPU
  opCodes["FADD"] = 0b1110000;
  opCodes["FSUB"] = 0b1110001;
  opCodes["FMUL"] = 0b1110010;
  opCodes["FDIV"] = 0b1110011;
  opCodes["FSQRT"] = 0b1110100;
  opCodes["ITOF"] = 0b1110101;
  opCodes["FTOI"] = 0b1110111;
  opCodes["FMOD"] = 0b1111000;

  //HACF -- HALT AND CATCH FIRE
  opCodes["HLT"] = 0b1111111;

  //////////////////////////////////////////
  /////// Number of Arguments of each opCode

  //Basic operationns
  numOfArgs["NOP"] = 0;
  numOfArgs["LDA"] = 1;
  numOfArgs["LDI"] = 1;
  numOfArgs["JMP"] = 1;
  numOfArgs["MI"] = 2;
  numOfArgs["MO"] = 2;

  //Conditional Jumps
  numOfArgs["JC"] = 1;
  numOfArgs["JO"] = 1;
  numOfArgs["JG"] = 1;
  numOfArgs["JGE"] = 1;
  numOfArgs["JE"] = 1;
  numOfArgs["JLE"] = 1;
  numOfArgs["JL"] = 1;
  numOfArgs["JNO"] = 1;
  numOfArgs["JZ"] = 1;
  numOfArgs["JNZ"] = 1;
  numOfArgs["JS"] = 1;
  numOfArgs["JNS"] = 1;

  // ALU
  numOfArgs["ADD"] = 3;
  numOfArgs["SUB"] = 3;
  numOfArgs["MUL"] = 3;
  numOfArgs["DIV"] = 3;
  numOfArgs["ADDI"] = 3;
  numOfArgs["SUBI"] = 3;
  numOfArgs["MULI"] = 3;
  numOfArgs["DIVI"] = 3;
  //Complex ALU operations
  numOfArgs["SQRT"] = 3;
  numOfArgs["MOD"] = 3;

  // Bitwise operationns
  numOfArgs["AND"] = 3;
  numOfArgs["OR"] = 3;
  numOfArgs["NAND"] = 3;
  numOfArgs["NOR"] = 3;
  numOfArgs["NOT"] = 3;
  numOfArgs["XOR"] = 3;
  numOfArgs["XOR"] = 3;
  numOfArgs["SHL"] = 3;
  numOfArgs["SHR"] = 3;
  numOfArgs["ROT"] = 3;

  //FPU
  numOfArgs["FADD"] = 3;
  numOfArgs["FSUB"] = 3;
  numOfArgs["FMUL"] = 3;
  numOfArgs["FDIV"] = 3;
  numOfArgs["FSQRT"] = 3;
  numOfArgs["ITOF"] = 3;
  numOfArgs["FTOI"] = 3;
  numOfArgs["FMOD"] = 3;

  //HACF -- HALT AND CATCH FIRE
  numOfArgs["HLT"] = 0;


  ////////////////////////////////
  //Registers
  reg["r1"] = 0b000;
  reg["r2"] = 0b001;
  reg["r3"] = 0b010;
  reg["r4"] = 0b011;
  reg["r5"] = 0b100;
  reg["r6"] = 0b101;
  reg["r7"] = 0b110;
  reg["r8"] = 0b111;

}

short Assembler::opCode(string s) {
  auto it = opCodes.find(s);

  if (it!=opCodes.end()) return it->second;
  else throw logic_error("Got \" " + s + "\" expected an opcode");
}

short Assembler::Register(string s) {
  auto it = reg.find(s);
  if (it!=reg.end()) return it->second;
  else throw logic_error(s + " is not a  register");
}

void Assembler::setInputFile(string infile) {
  cur_file = infile;
}

string tobinary(int n, int d = 16) {
  string s;
  while (d--) {
    s = std::to_string(n%2) + s;
    n = n >> 1;
  }
  return s;
}

bool Assembler::translate(string outName) {
  //get file name
  if (outName=="") outName = cur_file.substr(0, cur_file.find_last_of(".")) + ".bin";

  if (onDebug) cout << "Opening " << outName << " for writing ...";

  //open file for writing
  ofstream outfile;
  outfile.open(outName, ios::binary | ios::out);

  if (!outfile) {
    cout << "Cannot Open File\n\n";
    return false;
  }

  //Set All Vector values to zero
  for (int i = 0; i < SIZE; i++)
    rom.push_back(0);

  int romAddressPointer = 1;
  int ramAddressPointer = 1;
//  /* HEY CLEVER ARE YOU INSTRESTED IN ADDING THE RAM CONFIGURATION
//  for (auto& i : data){
//    bin[romAddressPointer++] = opCode["MI"] << (16 -  OPCODE_SIZE) + () ];
//  }

  for (auto inst : instructions) {                                             // parse every instruction
    int pos = 3;
    std::vector<std::string>::iterator arg_it = inst.arguments.begin();

    rom.at(romAddressPointer) = opCode(inst.opCode) << 9;

    if (!inst.arguments.size()) continue;                                    // opCodes with no args

    // Initialise Instructions taking 2 addresses lines
    if (inst.opCode=="MI" || inst.opCode=="MO" || inst.opCode=="STA" || inst.opCode=="STA") {

      for (arg_it; arg_it!=inst.arguments.end() - 1; arg_it++)            // loop though gives arguments
        rom.at(romAddressPointer) += (Register(*arg_it) << pos--*3);

      rom.at(++romAddressPointer) = Register(*arg_it);

    } else {
      for (auto arg_it : inst.arguments)
        rom.at(romAddressPointer) += (Register(arg_it) << pos--*3);
    }
    romAddressPointer++;
  }

  for (auto &i : text) {                                                     // add text
    rom.at(romAddressPointer++) = i.second;
  }

  for (auto adr : rom)
    outfile.write(reinterpret_cast<const char *>(&adr), sizeof(uint16_t));

  outfile.close();                                                           // Close file

  if (onDebug) {
    int row = 0;
    for (auto adr : rom) {
      cout << row++ << "\t";
      cout << adr << endl;
    }
  }
  return true;
}

std::string Assembler::getErrors() const {
  std::string out = "";

  for (auto e :errors) out.append(e.get());

  return out;
}

/*-------------------------------------------------- */
/* ---------------- Privare Methods ---------------- */
/*-------------------------------------------------- */

Assembler::instruction::instruction() : size{-1} {}

void Assembler::instruction::add(std::string s) {
  if (!++size) opCode = s;
  else arguments.push_back(s);
}

void Assembler::instruction::add(vector<std::string> &v) {
  arguments.reserve(arguments.size() + v.size());       //Memory Pre allocation
  arguments.insert(arguments.end(), v.begin(), v.end());
  size += v.size();
}

Assembler::error::error(int line, int index, string type = "", string message = "", string line_text = "")
    : line{line}, index{index}, type{type}, message{message}, line_text{line_text} {
}

const string Assembler::error::get() {
  return type + " on line " + to_string(line) + ":" + to_string(index) + "\t" + message;
}

bool Assembler::isEmptyLine(const string &line) {

  auto iter_start = line.begin();
  auto iter_end = line.end();

  bool success = phrase_parse(
      iter_start,
      iter_end,
      *rules::comment,
      space
  );

  return success && (iter_end==iter_start);
}

bool Assembler::isData(const string &line) {

  auto iter_start = line.begin();
  auto iter_end = line.end();

  bool success = parse(
      iter_start,
      iter_end,
      skip(space)[rules::data] >> *(space | rules::comment)
  );

  return success && (iter_end==iter_start);
}

bool Assembler::isText(const string &line) {

  auto iter_start = line.begin();
  auto iter_end = line.end();

  bool success = parse(
      iter_start,
      iter_end,
      skip(space)[rules::text] >> *(space | rules::comment)
  );

  return success && (iter_end==iter_start);
}

bool Assembler::isStart(const string &line) {

  auto iter_start = line.begin();
  auto iter_end = line.end();

  bool success = parse(
      iter_start,
      iter_end,
      skip(space)[rules::start] >> *(space | rules::comment)
  );

  return success && (iter_end==iter_start);
}

int Assembler::to_int(std::string &s) {
  if (s.find("0b")==0) {                            // binary
    return stoi(
        std::string(s.begin() + 2, s.end()),
        nullptr,
        2
    );
  } else if (s.find("0x")==0) {                    // Hexadecimal
    return stoi(
        std::string(s.begin() + 2, s.end()),
        nullptr,
        16
    );
  } else if (s=="true") return true;

  else if (s=="false") return false;

  else if (s[0]=='\'') return s[1];

  else return std::stoi(s);
}

bool Assembler::data_typeParser(const string &line, map<string, short> &stack) {

  short int_val = 0;

  //string iterators
  auto iter_start = line.begin();
  auto iter_end = line.end();

  // Bind
  data_type d;
  auto setType = [&](auto &ctx) { d.type = _attr(ctx); };
  auto setName = [&](auto &ctx) { d.name = _attr(ctx); };
  auto setVal = [&](auto &ctx) { d.value = _attr(ctx); };

  bool result = parse(
      iter_start,
      iter_end,
      skip(rules::comment | space)[rules::name[setType]
          >> rules::name[setName]
          >> -rules::value[setVal]]
          >> *(space | rules::comment)
  );

  if (result && iter_end!=iter_start) {
    result = false;
    errors.emplace_back(
        error(
            cur_line,
            iter_start - line.begin(),
            "Logic Error",
            "Unexpected -> " + string(iter_start, iter_end),
            line
        )
    );
  } else if (d.type=="int" || d.type=="bool" || d.type=="char" || d.type=="float") {

    int_val = to_int(d.value);

  } else if (opCodes.find(d.name)!=opCodes.end()
      || find(rules::types.begin(), rules::types.end(), d.name)!=rules::types.end()) {
    errors.emplace_back(
        error(
            cur_line,
            line.find(d.name),
            "Logic Error",
            d.name + " is not a valid identifier",
            line
        )
    );
  } else {
    errors.emplace_back(
        error(
            cur_line,
            iter_start - line.begin(),
            "Syntax Error",
            d.type + " does not name a type",
            line
        )
    );
    result = false;
  }

  if (onDebug) cout << d.type << ": " << d.name << "\n\t->" << d.value << " (" << int_val << ")" << endl;

  if (result) stack[d.name] = int_val;


  return result;

}

bool Assembler::lineParser(const string &line) {

  //string iterators
  auto iter_start = line.begin();
  auto iter_end = line.end();

  instruction inst;

  // Bind
  auto add = [&](auto &ctx) { inst.add(_attr(ctx)); };

  bool result = parse(
      iter_start,
      iter_end,
      skip(rules::comment | space)[rules::name[add]
          >> *(rules::name[add]%',')]
          >> *(space | rules::comment)
  );

  if (result && inst.size && opCodes.find(inst.opCode)==opCodes.end()) {    // given identifier is not valid opCode
    result = false;

    errors.emplace_back(
        error(cur_line,
              iter_end - iter_start,
              "Syntax Error",
              inst.opCode + " is not a valid opcode.",
              line
        )
    );
  } else if (result && inst.size!=numOfArgs[inst.opCode]) {                 // Check arguments number

    result = false;

    errors.emplace_back(
        error(cur_line,
              iter_start - line.begin(),
              "Logic Error",
              inst.opCode + " expects " + to_string(numOfArgs[inst.opCode]) + " argument(s), got " +
                  to_string(inst.size) + ".",    // Example: ADD expects 3 arguments, got 4
              line
        )
    );
  }

  if (iter_end!=iter_start) {                                               // Create an error if parser

    result = false;

    errors.emplace_back(
        error(cur_line,
              iter_start - line.begin(),
              "Syntax Error",
              "Unexpected -> " + string(iter_start, iter_end),
              line
        )
    );
  }

  // Code used for debugging
  if (onDebug) {
    cout << cur_line << ": " << inst.opCode << "(" << numOfArgs[inst.opCode] << ")" << endl;

    for (auto i : inst.arguments)
      cout << "\t->" << i << std::endl;
  }

  // Add instruction to stack
  if (result) instructions.push_back(inst);

  return result;
}

bool Assembler::parser(const string &s) {

  cur_line = 0;

  stringstream stream(s);
  string line;
  while (std::getline(stream, line)) {
    cur_line++;
    lineParser(line);
  }

}   // I might fix this later, depreciated for now

bool Assembler::parser(ifstream &fs) {

  bool parseData = false;
  bool parseText = false;
  bool parseStart = false;

  cur_line = 0;
  string line;
  bool success = true;

  while (std::getline(fs, line)) {

    cur_line++;

    if (isEmptyLine(line)) continue;          //skip empty lines

    if (isData(line)) {
      if (parseData) {                        // Return Error if Already have parsed Data section
        errors.emplace_back(
            error(cur_line,
                  line.find('s'),             // Find where section starts
                  "Logic Error",
                  "Redefinition of .data ,already defined at " + std::to_string(parseData),
                  line
            )
        );
        success = false;
      } else parseData = true;
      continue;
    }

    if (isText(line)) {
      if (parseText) {                           // Return Error if Already have parsed Text section
        errors.emplace_back(
            error(cur_line,
                  line.find('s'),                // Find where section starts
                  "Logic Error",
                  "Redefinition of .text ,already defined at " + std::to_string(parseData),
                  line
            )
        );
        success = false;
      } else parseText = true;
      continue;
    }

    if (isStart(line)) {
      if (parseStart) {                           // Return Error if Already have parsed Start section
        errors.emplace_back(
            error(cur_line,
                  line.find('s'),                 // Find where section starts
                  "Logic Error",
                  "Redefinition of .text ,already defined at " + std::to_string(parseData),
                  line
            )
        );
        success = false;
      } else parseStart = true;
      continue;
    }

    if (parseStart)
      success = lineParser(line) && success;

    else if (parseText)
      success = data_typeParser(line, text) && success;

    else if (parseData)
      success = data_typeParser(line, data) && success;

    else {
      errors.emplace_back(                                //throw error for instructions/declerations outside of sections
          error(cur_line,
                line.find('s'),
                "Syntax Error",
                "Unexpeced outside of section",
                line
          )
      );
      success = false;
    }

  } //end of parse loop


  if (success) text[".start"] = data.size();             // Move .start to pointer set memory values on start

  if (!success && onDebug)
    for (auto e : errors)
      cout << e.get() << endl;

  if (onDebug) {                                          // Print Variables values onDebug mode
    cout << "_______" << " data " << "_______" << endl;
    for (auto &i:data)
      cout << i.first << " : " << i.second << endl;

    cout << "_______" << " text " << "_______" << endl;
    for (auto &i:text)
      cout << i.first << " : " << i.second << endl;
  }

  return success;
}

} //End of asmbl namesapce