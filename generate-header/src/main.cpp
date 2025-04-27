#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Escapes backslashes and quotes for C++ string literals
static std::string escapeForLiteral(const std::string& s) {
	std::string out;
	out.reserve(s.size());
	for (char c : s) {
		if (c == '\\') out += "\\\\";
		else if (c == '"') out += "\\\"";
		else            out += c;
	}
	return out;
}

static void printHelp(const char* exeName) {
	std::cout << "Usage: " << exeName << " [data_file]\n"
		<< "\n"
		<< "Reads a configuration file listing Directories and RegistryKeys, then generates data.h with QStringList literals.\n"
		<< "If no data_file is provided, it defaults to 'data.txt' in the executable directory.\n"
		<< "Options:\n"
		<< "  -h, -help     Show this help message and exit.\n";
}

int main(int argc, char* argv[]) {
	const char* defaultFile = "data.txt";
	const char* fileName = nullptr;

	if (argc == 1) {
		fileName = defaultFile;
	}
	else if (argc == 2) {
		std::string arg = argv[1];
		if (arg == "-h" || arg == "-help") {
			printHelp(argv[0]);
			return 0;
		}
		else if (!arg.empty() && arg[0] == '-') {
			std::cerr << "Unknown option '" << arg << "'. Use -h or -help for usage.\n";
			return 1;
		}
		else {
			fileName = argv[1];
		}
	}
	else {
		std::cerr << "Too many arguments. Use -h or -help for usage.\n";
		return 1;
	}

	std::ifstream in(fileName);
	if (!in) {
		std::cerr << "Could not open '" << fileName << "'.\n";
		return 1;
	}

	enum Section { NONE, DIRS, REGS } sec = NONE;
	std::vector<std::string> dirs, regs;
	std::string line;

	while (std::getline(in, line)) {
		// Trim whitespace
		auto l = line.find_first_not_of(" \t\r\n");
		if (l == std::string::npos) continue;
		auto r = line.find_last_not_of(" \t\r\n");
		std::string t = line.substr(l, r - l + 1);

		if (t.size() >= 2 && t.front() == '[' && t.back() == ']') {
			std::string tag = t.substr(1, t.size() - 2);
			if (tag == "Directories")      sec = DIRS;
			else if (tag == "RegistryLeys") sec = REGS;
			else sec = NONE;
		}
		else if (t.size() >= 2 && t.front() == '"' && t.back() == '"') {
			std::string content = t.substr(1, t.size() - 2);
			if (sec == DIRS) dirs.push_back(content);
			if (sec == REGS) regs.push_back(content);
		}
	}
	in.close();

	std::ofstream out("data.h");
	if (!out) {
		std::cerr << "Could not create data.h.\n";
		return 2;
	}

	out << "#pragma once\n\n";
	out << "#include <QStringList>\n\n";

	out << "// List of paths (directories)\n";
	out << "inline const QStringList paths = {\n";
	for (const auto& d : dirs) {
		out << "    \"" << escapeForLiteral(d) << "\"," << "\n";
	}
	out << "};\n\n";

	out << "// List of registry keys\n";
	out << "inline const QStringList registryKeys = {\n";
	for (const auto& k : regs) {
		out << "    \"" << escapeForLiteral(k) << "\"," << "\n";
	}
	out << "};\n";

	out.close();
	std::cout << "Generated data.h successfully.\n";
	return 0;
}
