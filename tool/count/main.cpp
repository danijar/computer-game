#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <iostream>

using namespace std;
using namespace std::tr2::sys;


void Print(string Message)
{
	cout << Message << endl;
}

vector<string> explode(string input, string delimiter)
{
	vector<string> explodes;
	input.append(delimiter);
	while(input.find(delimiter) != string::npos)
	{
		explodes.push_back(input.substr(0, input.find(delimiter)));
		input.erase(input.begin(), input.begin() + input.find(delimiter) + delimiter.size());
	}
	return explodes;
}

string trim(string input, string characters = " \"")
{
	if(!input.size()) return "";
	string::size_type start = input.find_first_not_of(characters);
	string::size_type end   = input.find_last_not_of(characters) + 1;
	return input.substr(start, end);
}

struct statistic
{
	statistic() : folders(0) {}
	int folders;
	unordered_map<string, int> files;
	unordered_map<string, int> lines;
};

string align(string input, int length = 7, bool right = false)
{
	string output = "";

	if(!right) output += input;
	for(int i = 0; i < length - (signed int)input.size(); ++i) output += " ";
	if(right) output += input;

	return output;
}

string align_int(int input, int length = 7, bool right = false)
{
	return align(to_string(input), length, right);
}

void Output(string Name, statistic *S, vector<string> Extensions = vector<string>())
{
	auto Line = [&](unsigned int length, char symbol){ string line = ""; for(unsigned int j = 0; j < length; ++j) line += symbol; Print(line); };

	Print("");
	Print(Name);
	Line(35, '-');

	int overallfiles = 0, sourcefiles = 0, otherfiles = 0;
	int overalllines = 0, sourcelines = 0, otherlines = 0;
	for(auto j : S->files)
	{
		string e = j.first;

		int files = j.second;
		int lines = S->lines.find(e) == S->lines.end() ? 0 : S->lines[e];

		if(find(Extensions.begin(), Extensions.end(), e) != Extensions.end())
		{
			Print(align(e, 7) + " " + align_int(files, 7, true) + " files " + align_int(lines, 7, true) + " lines");
			sourcefiles += files;
			sourcelines += lines;
		}
		else
		{
			otherfiles += files;
			otherlines += lines;
		}

		overallfiles += files;
		overalllines += lines;
	}

	Print("");
	Print(align("folders", 7) + " " + align_int(S->folders, 7, true));
	Print(align("source",  7) + " " + align_int(sourcefiles,  7, true) + " files " + align_int(sourcelines,  7, true) + " lines");
	Print(align("other",   7) + " " + align_int(otherfiles,   7, true) + " files " + align_int(otherlines,   7, true) + " lines");
	Print(align("overall", 7) + " " + align_int(overallfiles, 7, true) + " files " + align_int(overalllines, 7, true) + " lines");
	Print("");
}

string program(int argc, char* argv[])
{
	// options
	string directory = "./";
	vector<string> includes;
	vector<string> extensions;

	// parse command line parameters
	for(int i = 0; i < argc; ++i)
	{
		string arg(argv[i]);
		// working directory
		if(arg == "-dir")
		{
			if(i + 1 > argc - 1) return "Expected directory following -dir switch.";
			directory = string(argv[++i]);
		}
		// includesd directories
		else if(arg == "-include")
		{
			if(i + 1 > argc - 1) return "Expected comma separated list of directorys following -includes switch.";
			includes = explode(string(argv[++i]), ",");
		}
		// extensions of file to count lines of
		else if(arg == "-extension")
		{
			if(i + 1 > argc - 1) return "Expected comma separated list of file extensions following -extension switch.";
			extensions = explode(string(argv[++i]), ",");
		}
	}

	// parameter defaults
	if(!includes.size())
		includes.push_back("");
	if(!extensions.size())
	{
		extensions.push_back("h");
		extensions.push_back("hpp");
		extensions.push_back("hxx");
		extensions.push_back("cpp");
		extensions.push_back("cxx");
	}

	// prepare parameters
	directory = trim(directory);
	for(auto i = includes.begin(); i != includes.end(); ++i)
		*i = trim(*i);

	// statistics
	unordered_map<string, statistic*> statistics;
	for(auto i : includes)
		statistics.insert(make_pair(i, new statistic()));

	// loop through directories
	for(auto include : includes)
	{
		for(auto i = recursive_directory_iterator(path(directory + "/" + include)); i != recursive_directory_iterator(); ++i)
		{
			auto p = i->path();
			if(is_directory(p))
			{
				statistics[include]->folders++;
			}
			else if(is_regular_file(p))
			{
				string e = trim(p.extension(), " .");
				statistics[include]->files[e]++;

				string ignore;
				ifstream file(p);
				while (getline(file, ignore))
					statistics[include]->lines[e]++;
			}
			//Print(p.filename());
		}
	}

	// print results
	for(auto i : statistics)
		Output(directory + i.first, i.second, extensions);
	statistic overall;
	for(auto i : statistics)
	{
		overall.folders += i.second->folders;
		for(auto j : i.second->files)
			overall.files[j.first] += j.second;
		for(auto j : i.second->lines)
			overall.lines[j.first] += j.second;
	}
	Output("overall", &overall, extensions);
	return "";
}

int main(int argc, char* argv[])
{
	Print(program(argc, argv));
	return EXIT_SUCCESS;
}
