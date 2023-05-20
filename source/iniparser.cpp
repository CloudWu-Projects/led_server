#include "iniparser.h"
#include <fmt/format.h>
#include <sstream>
//
//#include "hdef.h"
//#include "herr.h"
//#include "hstring.h"
//#include "hfile.h"
//#include "hbase.h"


/**********************************
# div

[section]

key = value # span

# div
***********************************/

class IniNode {
public:
    enum Type {
        INI_NODE_TYPE_UNKNOWN,
        INI_NODE_TYPE_ROOT,
        INI_NODE_TYPE_SECTION,
        INI_NODE_TYPE_KEY_VALUE,
        INI_NODE_TYPE_DIV,
        INI_NODE_TYPE_SPAN,
    } type;
    std::string  label; // section|key|comment
    std::string  value;
    std::list<IniNode*>    children;

    virtual ~IniNode() {
        for (auto pNode : children) {
            if (pNode) {
                delete pNode;
            }
        }
        children.clear();
    }

    void Add(IniNode* pNode) {
        children.push_back(pNode);
    }

    void Del(IniNode* pNode) {
        for (auto iter = children.begin(); iter != children.end(); ++iter) {
            if ((*iter) == pNode) {
                delete (*iter);
                children.erase(iter);
                return;
            }
        }
    }

    IniNode* Get(const std::string& label, Type type = INI_NODE_TYPE_KEY_VALUE) {
        for (auto pNode : children) {
            if (pNode->type == type && pNode->label == label) {
                return pNode;
            }
        }
        return NULL;
    }
};

class IniSection : public IniNode {
public:
    IniSection() : IniNode(), section(label) {
        type = INI_NODE_TYPE_SECTION;
    }
    std::string &section;
};

class IniKeyValue : public IniNode {
public:
    IniKeyValue() : IniNode(), key(label) {
        type = INI_NODE_TYPE_KEY_VALUE;
    }
    std::string &key;
};

class IniComment : public IniNode {
public:
    IniComment() : IniNode(), comment(label) {
    }
    std::string &comment;
};

IniParser::IniParser() {
    _comment = DEFAULT_INI_COMMENT;
    _delim = DEFAULT_INI_DELIM;
    root_ = NULL;
}

IniParser::~IniParser() {
    Unload();
}

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  do {if (p) {delete (p); (p) = NULL;}} while(0)
#endif
int IniParser::Unload() {
    SAFE_DELETE(root_);
    return 0;
}

int IniParser::Reload() {
    return LoadFromFile(_filepath.c_str());
}

int IniParser::LoadFromFile(const char* filepath) {
    _filepath = filepath;

    FILE* fp;
    fp = fopen(filepath, "r");
    if (fp == nullptr) {
        return -1;
    }

    std::string str;

    fseek(fp, 0, SEEK_END);
    int filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    str.resize(filesize);
    fread((void*)str.data(), 1, filesize, fp);
    fclose(fp);
    const char* c_str = str.c_str();
    unsigned char utf8_bom[3] = { 0xEF, 0xBB, 0xBF };
    if (str.size() >= 3 && memcmp(c_str, utf8_bom, 3) == 0) {
        c_str += 3;
    }
    return LoadFromMem(c_str);
}

#define SPACE_CHARS     " \t\r\n"
#define PAIR_CHARS      "{}[]()<>\"\"\'\'``"

std::string ltrim(const std::string& str, const char* chars = SPACE_CHARS) {
	std::string::size_type pos = str.find_first_not_of(chars);
	if (pos == std::string::npos)    return "";
	return str.substr(pos);
}

std::string rtrim(const std::string& str, const char* chars = SPACE_CHARS) {
	std::string::size_type pos = str.find_last_not_of(chars);
	return str.substr(0, pos + 1);
}
std::string trim(const std::string& str, const char* chars = SPACE_CHARS) {
	std::string::size_type pos1 = str.find_first_not_of(chars);
	if (pos1 == std::string::npos)   return "";

	std::string::size_type pos2 = str.find_last_not_of(chars);
	return str.substr(pos1, pos2 - pos1 + 1);
}

int IniParser::LoadFromMem(const char* data) {
    Unload();

    root_ = new IniNode;
    root_->type = IniNode::INI_NODE_TYPE_ROOT;

    std::stringstream ss;
    ss << data;
    std::string strLine;
    int line = 0;
    std::string::size_type pos;

    std::string content, comment, strDiv;
    IniNode* pScopeNode = root_;
    IniNode* pNewNode = NULL;
    while (std::getline(ss, strLine)) {
        ++line;

        content = ltrim(strLine);
        if (content.length() == 0)  {
            // blank line
            strDiv += '\n';
            continue;
        }

        // trim_comment
        comment = "";
        pos = content.find_first_of(_comment);
        if (pos != std::string::npos) {
            comment = content.substr(pos);
            content = content.substr(0, pos);
        }

        content = rtrim(content);
        if (content.length() == 0) {
            strDiv += strLine;
            strDiv += '\n';
            continue;
        } else if (strDiv.length() != 0) {
            IniNode* pNode = new IniNode;
            pNode->type = IniNode::INI_NODE_TYPE_DIV;
            pNode->label = strDiv;
            pScopeNode->Add(pNode);
            strDiv = "";
        }

        if (content[0] == '[') {
            if (content[content.length()-1] == ']') {
                // section
                content = trim(content.substr(1, content.length()-2));
                pNewNode = new IniNode;
                pNewNode->type = IniNode::INI_NODE_TYPE_SECTION;
                pNewNode->label = content;
                root_->Add(pNewNode);
                pScopeNode = pNewNode;
            } else {
                // hlogw("format error, line:%d", line);
                continue;   // ignore
            }
        } else {
            pos = content.find_first_of(_delim);
            if (pos != std::string::npos) {
                // key-value
                pNewNode = new IniNode;
                pNewNode->type = IniNode::INI_NODE_TYPE_KEY_VALUE;
                pNewNode->label = trim(content.substr(0, pos));
                pNewNode->value = trim(content.substr(pos+_delim.length()));
                pScopeNode->Add(pNewNode);
            } else {
                // hlogw("format error, line:%d", line);
                continue;   // ignore
            }
        }

        if (comment.length() != 0) {
            // tail_comment
            IniNode* pNode = new IniNode;
            pNode->type = IniNode::INI_NODE_TYPE_SPAN;
            pNode->label = comment;
            pNewNode->Add(pNode);
            comment = "";
        }
    }

    // file end comment
    if (strDiv.length() != 0) {
        IniNode* pNode = new IniNode;
        pNode->type = IniNode::INI_NODE_TYPE_DIV;
        pNode->label = strDiv;
        root_->Add(pNode);
    }

    return 0;
}

void IniParser::DumpString(IniNode* pNode, std::string& str) {
    if (pNode == NULL)  return;

    if (pNode->type != IniNode::INI_NODE_TYPE_SPAN) {
        if (str.length() > 0 && str[str.length()-1] != '\n') {
            str += '\n';
        }
    }

    switch (pNode->type) {
    case IniNode::INI_NODE_TYPE_SECTION: {
        str += '[';
        str += pNode->label;
        str += ']';
    }
    break;
    case IniNode::INI_NODE_TYPE_KEY_VALUE: {
        str += fmt::format ("{} {} {}", pNode->label, _delim, pNode->value);
    }
    break;
    case IniNode::INI_NODE_TYPE_DIV: {
        str += pNode->label;
    }
    break;
    case IniNode::INI_NODE_TYPE_SPAN: {
        str += '\t';
        str += pNode->label;
    }
    break;
    default:
    break;
    }

    for (auto p : pNode->children) {
        DumpString(p, str);
    }
}

std::string IniParser::DumpString() {
    std::string str;
    DumpString(root_, str);
    return str;
}

int IniParser::Save() {
    return SaveAs(_filepath.c_str());
}

int IniParser::SaveAs(const char* filepath) {
    std::string str = DumpString();
    if (str.length() == 0) {
        return 0;
    }

    FILE* fp;
    fp = fopen(filepath, "w");
    if (fp==nullptr) {
        return -1;
    }
    fwrite(str.c_str(), str.length(),1,fp);
    fclose(fp);

    return 0;
}

std::list<std::string> IniParser::GetSections() {
    std::list<std::string> ret;
    if (root_ == NULL) return std::move(ret);

    for (auto pNode : root_->children) {
        if (pNode->type == IniNode::INI_NODE_TYPE_SECTION) {
            ret.push_back(pNode->label);
        }
    }
    return std::move(ret);
}

std::list<std::string> IniParser::GetKeys(const std::string& section) {
    std::list<std::string> ret;
    if (root_ == NULL) return std::move(ret);

    IniNode* pSection = root_;
    if (section.length() != 0) {
        pSection = root_->Get(section, IniNode::INI_NODE_TYPE_SECTION);
        if (pSection == NULL) return std::move(ret);
    }

    for (auto pNode : pSection->children) {
        if (pNode->type == IniNode::INI_NODE_TYPE_KEY_VALUE) {
            ret.push_back(pNode->label);
        }
    }
    return std::move(ret);
}

std::string IniParser::GetValue(const std::string& key, const std::string& section) {
    if (root_ == NULL)  return "";

    IniNode* pSection = root_;
    if (section.length() != 0) {
        pSection = root_->Get(section, IniNode::INI_NODE_TYPE_SECTION);
        if (pSection == NULL)   return "";
    }

    IniNode* pKV = pSection->Get(key, IniNode::INI_NODE_TYPE_KEY_VALUE);
    if (pKV == NULL)    return "";

    return pKV->value;
}

void IniParser::SetValue(const std::string& key, const std::string& value, const std::string& section) {
    if (root_ == NULL) {
        root_ = new IniNode;
    }

    IniNode* pSection = root_;
    if (section.length() != 0) {
        pSection = root_->Get(section, IniNode::INI_NODE_TYPE_SECTION);
        if (pSection == NULL) {
            pSection = new IniNode;
            pSection->type = IniNode::INI_NODE_TYPE_SECTION;
            pSection->label = section;
            root_->Add(pSection);
        }
    }

    IniNode* pKV = pSection->Get(key, IniNode::INI_NODE_TYPE_KEY_VALUE);
    if (pKV == NULL) {
        pKV = new IniNode;
        pKV->type = IniNode::INI_NODE_TYPE_KEY_VALUE;
        pKV->label = key;
        pSection->Add(pKV);
    }
    pKV->value = value;
}
#define HV_EXPORT

bool hv_getboolean(const char* str) {
	if (str == NULL) return false;
	int len = strlen(str);
	if (len == 0) return false;
	switch (len) {
	case 1: return *str == '1' || *str == 'y' || *str == 'Y';
	case 2: return stricmp(str, "on") == 0;
	case 3: return stricmp(str, "yes") == 0;
	case 4: return stricmp(str, "true") == 0;
	case 6: return stricmp(str, "enable") == 0;
	default: return false;
	}
}

template<>
HV_EXPORT bool IniParser::Get(const std::string& key, const std::string& section, bool defvalue) {
    std::string str = GetValue(key, section);
    return str.empty() ? defvalue : hv_getboolean(str.c_str());
}

template<>
HV_EXPORT int IniParser::Get(const std::string& key, const std::string& section, int defvalue) {
    std::string str = GetValue(key, section);
    return str.empty() ? defvalue : atoi(str.c_str());
}

template<>
HV_EXPORT float IniParser::Get(const std::string& key, const std::string& section, float defvalue) {
    std::string str = GetValue(key, section);
    return str.empty() ? defvalue : atof(str.c_str());
}

template<>
HV_EXPORT void IniParser::Set(const std::string& key, const bool& value, const std::string& section) {
    SetValue(key, value ? "true" : "false", section);
}

template<>
HV_EXPORT void IniParser::Set(const std::string& key, const int& value, const std::string& section) {
    SetValue(key, fmt::format("{}", value), section);
}

template<>
HV_EXPORT void IniParser::Set(const std::string& key, const float& value, const std::string& section) {
    SetValue(key, fmt::format("{}", value), section);
}
