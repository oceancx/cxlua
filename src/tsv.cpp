#include "tsv.h"
#include <fstream>
#include <sstream>
using String = std::string;
using Json = nlohmann::json;
namespace utils
{
	tsv::tsv(String path) 
	{
		std::ifstream fs(path);
		if (!fs)
		{
			//cxlog_err("tsv(String path) error!");
			return;
		};

		fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		std::stringstream  ss;
		ss << fs.rdbuf();
		fs.close();
		
		std::map<String, String> defaultRow;
		std::string file_content = ss.str();
		std::vector<String> rows = utils::split(file_content, '\n');
		Rows.clear();
		if (rows.size() > 0)
		{
			Keys = utils::split_by_cuts(rows[0], '\t');
			for (size_t i = 1; i < rows.size(); i++)
			{
				if (rows[i][0] == '*')
				{
					rows[i] = rows[i].substr(1);
					std::vector<std::string> vals = split_by_cnt(rows[i], '\t',(int) Keys.size());
					for (size_t ki = 0; ki < Keys.size(); ki++)
					{
						defaultRow[Keys[ki]] = vals[ki];
					}
					continue;
				}
				
				std::vector<std::string> vals = split_by_cnt(rows[i], '\t',(int)Keys.size());
				std::map<String, String> tsvrow;
				for (size_t ki = 0; ki < Keys.size(); ki++)
				{
					String key = Keys[ki];
					tsvrow[key] = vals[ki] != "" ? vals[ki] : defaultRow[key];
				}
				Rows.push_back(tsvrow);
			}
		}
		MapRows.clear();
		for (auto row: Rows)
		{
			MapRows.insert({ row["ID"], row });
		}
		
		//nlohmann::json json = nlohmann::json::array();
		//json = tabRows;
		//jsonstr = json.dump();

	}

	tsv::~tsv()
	{

	}

	uint32_t tsv_parse_wasstr(std::string s)
	{
		if (s == "")return 0;
		return std::stoul(s, 0, 16);
	}
}

int utils_parse_tsv_file(lua_State*L )
{
	const char* path = luaL_checklstring(L, 1, NULL);
	std::ifstream fs(path);
	if (!fs)
	{
		printf("utils_parse_tsv_file read file error!");
		return 0;
	};

	fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::stringstream  ss;
	ss << fs.rdbuf();
	fs.close();

	std::string file_content = ss.str();
	std::vector<std::string> rows = utils::split(file_content, '\n');
	if (rows.size() > 0)
	{
		std::vector<std::string> keys = utils::split(rows[0], '\t');
		if (keys.size() > 0)
		{
			Json tsv_parsed_rows = Json();
			int tsv_index = 1;
			for (size_t i = 1; i < rows.size(); i++)
			{
				if(rows[i][0] == '*')continue;
				Json& tsv_row = tsv_parsed_rows[std::to_string(tsv_index++)];
				std::vector<std::string> vals = utils::split(rows[i], '\t');
				assert(keys.size() == vals.size());
				
				for (size_t j = 0; j < keys.size(); j++)
				{
					tsv_row[keys[j]] = vals[j];
				}
			}
			lua_pushstring(L, tsv_parsed_rows.dump().c_str());
			return 1;
		}
	}
	
	return 0;
}

void utils_resave_tsv_file(const char* path)
{	
	std::ifstream fs(path);
	if (!fs)
	{
		printf("read file error!");
		return ;
	};

	fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::stringstream  ss;
	ss << fs.rdbuf();
	fs.close();

	std::string file_content = ss.str();
	std::vector<std::string> rows = utils::split(file_content, '\n');
	if (rows.size() > 0)
	{
		std::vector<std::string> keys = utils::split(rows[0], '\t');
		if (keys.size() > 0)
		{
			std::vector<std::map<std::string, std::string>> parsedrows;
			for (size_t i = 1; i < rows.size(); i++)
			{
				if (rows[i][0] == '*')continue;
				std::map<std::string, std::string> parsedrow;		
				std::vector<std::string> vals = utils::split(rows[i], '\t');
				assert(keys.size() == vals.size());

				for (size_t j = 0; j < keys.size(); j++)
				{
					parsedrow[keys[j]] = vals[j];
				}
				parsedrows.push_back(parsedrow);
			}
			std::sort(parsedrows.begin(), parsedrows.end(), [](std::map<std::string, std::string>& lhs, std::map<std::string, std::string>& rhs) {
				return lhs["name"] < rhs["name"];
			});
			std::ofstream newfs(path);
			newfs << rows[0] << '\n';		//write keys
			for (size_t i = 0; i < parsedrows.size(); i++)
			{
				bool first = true;
				for (auto& it : parsedrows[i])
				{
					if (first)
					{
						first = false;
						newfs << it.second;
					}
					else
					{
						newfs << '\t' << it.second;
					}
				}
				newfs << '\n';
			}
			newfs.close();
		}
	}
}

//0不是number 1是整数 2是浮点数
int utils_str_is_number(const std::string& str)
{
	std::stringstream ss(str);
	double d;
	char c;
	if (!(ss >> d))
		return 0;
	if (ss >> c)
		return 0;
	int intd = (int)d;
	if (d == intd)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}


int utils_parse_tsv_file_as_table(lua_State*L)
{
	const char* path = luaL_checkstring(L, 1);
	bool parse_val_type= lua_toboolean(L, 2);

	
	std::ifstream fs(path);
	if (!fs)
	{
		printf("utils_parse_tsv_file read file error!");
		return 0;
	};

	fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::stringstream  ss;
	ss << fs.rdbuf();
	fs.close();

	std::string file_content = ss.str();
	std::vector<std::string> rows = utils::split(file_content, '\n');
	
	if (rows.size() > 0)
	{
		std::vector<std::string> keys = utils::split(rows[0], '\t');
		//generate a new table
		lua_newtable(L);
		int table_index = 1;
		for (size_t i = 1; i < rows.size(); i++)
		{
			if (rows[i][0] == '*')continue;
			//generate a row
			std::vector<std::string> vals = utils::split(rows[i], '\t');
			lua_pushinteger(L, table_index++);
			lua_newtable(L);
			for (size_t j = 0; j < keys.size() ; j++)
			{
				const auto& key = keys[j];
				const auto& val = (j < vals.size()) ? vals[j] : std::string("");
				
				if (parse_val_type)
				{
					int val_type = utils_str_is_number(val);
					if (val_type == 0)
					{
						lua_pushargs(L, val.c_str());
					}
					else if (val_type == 1)
					{
						lua_pushargs(L, std::atoi(val.c_str()));
					}
					else if (val_type == 2)
					{
						lua_pushargs(L, std::atof(val.c_str()));
					}
				}
				else
				{
					lua_pushargs(L, val.c_str());
				}

				lua_setfield(L, -2, key.c_str());
			}
			lua_settable(L, -3);
		}
		return 1;
	}
	else
	{
		return 0;
	}

}

int utils_str_split(lua_State*L)
{
	const char* str = luaL_checkstring(L, 1);
	const char* delim = luaL_checkstring(L, 2);
	if (str == nullptr || delim == nullptr)return 0;
	std::string _str(str);
	char _delim1(delim[0]);
	char _delim2 = 0; 
	if (strlen(delim) > 1)
	{
		_delim2 = (delim[1]);
	}

	auto splitstrs = utils::split(_str, _delim1);
	if (splitstrs.size() > 0)
	{
		lua_newtable(L);
		int table_index = 1;
		for (auto& s : splitstrs)
		{
			if (_delim2 != 0)
			{
				auto splitstrs2 = utils::split(s, _delim2);
				for (auto& s2 : splitstrs2)
				{
					lua_pushinteger(L, table_index++);
					lua_pushargs(L, s2.c_str());
					lua_settable(L, -3);
				}
			}
			else
			{
				lua_pushinteger(L, table_index++);
				lua_pushargs(L, s.c_str());
				lua_settable(L, -3);
			}
		}
		return 1;
	}
	else
	{
		lua_newtable(L);
		lua_pushinteger(L, 1);
		lua_pushargs(L, str);
		lua_settable(L, -2);
		return 1;
	}
}

int utils_file_open(lua_State*L)
{
	const char* path = luaL_checkstring(L, 1);
	int mode =(int) luaL_optinteger(L, 2 , std::fstream::in|std::fstream::out);
	static std::fstream fs;
	if (fs&&fs.is_open())
	{
		fs.close();
	}

	fs.open(path, mode);
	if (!fs)
		return 0;
	lua_pushlightuserdata(L,(void*)&fs);
	return 1;
}

int utils_file_close(lua_State*L)
{
	void* udata = lua_touserdata(L, 1);
	if (!udata)return 0;
	std::fstream* fsptr = (std::fstream*)udata;
	std::fstream& fs = *fsptr;
	if (fs&&fs.is_open())
	{
		fs.close();
	}
	return 0;
}

int utils_file_write(lua_State*L)
{
	void* udata = lua_touserdata(L, 1);
	if (!udata)return 0;
	std::fstream* fsptr = (std::fstream*)udata;
	std::fstream& fs = *fsptr;
	if (fs&&fs.is_open())
	{
		const char* data = luaL_checkstring(L, 2);
		fs << data;
	}
	return 0;
}


void luaopen_tsv(lua_State* L)
{
	script_system_register_luac_function(L, utils_parse_tsv_file);
	script_system_register_luac_function(L, utils_parse_tsv_file_as_table);
	script_system_register_luac_function(L, utils_str_split);
	script_system_register_luac_function(L, utils_file_open);
	script_system_register_luac_function(L, utils_file_close);
	script_system_register_luac_function(L, utils_file_write);
	script_system_register_function(L, utils_resave_tsv_file);
}

