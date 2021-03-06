#pragma once

#include <stdio.h>
#include <string>
#include "rapidxml.hpp"
#include "MZFileSystem.h"

using MXmlDomDoc = rapidxml::xml_document<>;
using MXmlDomDocPtr = MXmlDomDoc*;
using MXmlDomNodePtr = rapidxml::xml_node<>*;
using MXmlDomElementPtr = rapidxml::xml_node<>*;
using MXmlDomPIPtr = rapidxml::xml_node<>*;
using MXmlDomNamedNodeMapPtr = rapidxml::xml_node<>*;
using MXmlDomTextPtr = rapidxml::xml_node<>*;
using MXmlDomParseError = rapidxml::parse_error;
using MXmlDomNodeType = rapidxml::node_type;

class MXmlDocument;

class MXmlNode
{
protected:
	MXmlDomNodePtr		m_pDomNode{};

public:
	MXmlNode() = default;
	MXmlNode(MXmlDomNodePtr m_pDomNode) : m_pDomNode{ m_pDomNode } {}

	MXmlDomNodePtr	GetXmlDomNodePtr() const { return m_pDomNode; }
	void			SetXmlDomNodePtr(MXmlDomNodePtr pNode) { m_pDomNode = pNode; }

	bool IsEmpty() const { return m_pDomNode == nullptr; }

	template<size_t size> void GetNodeName(char(&sOutStr)[size]) {
		GetNodeName(sOutStr, size);
	}
	void GetNodeName(char* sOutStr, int maxlen);
	void GetText(char* sOutStr, int nMaxCharNum = -1);
	void SetText(const char* sText);
	
	int	GetChildNodeCount();
	MXmlDomNodeType GetNodeType();
	bool HasChildNodes();

	void NextSibling();
	void PreviousSibling();

	bool AppendChild(MXmlNode node);

	bool FindChildNode(const char* sNodeName, MXmlNode* pOutNode);

	MXmlNode GetParent() { if (m_pDomNode) return MXmlNode(m_pDomNode->parent()); else return MXmlNode(); }
	MXmlNode GetChildNode(int iIndex);
};

class MXmlElement : public MXmlNode
{
public:
	MXmlElement() = default;
	MXmlElement(MXmlNode Node) : MXmlNode{ Node } {}
	using MXmlNode::MXmlNode;

	template<size_t size> void GetTagName(char(&sOutStr)[size]) {
		GetTagName(sOutStr, size);
	}
	void GetTagName(char* sOutStr, int maxlen) { MXmlNode::GetNodeName(sOutStr, maxlen); }
	
	void GetContents(char* sOutStr) { MXmlNode::GetText(sOutStr); }
	void GetContents(int* ipOutValue);
	void GetContents(bool* bpOutValue);
	void GetContents(float* fpOutValue);
	void GetContents(std::string* pstrValue);

	void SetContents(const char* sStr) { MXmlNode::SetText(sStr); }
	void SetContents(int iValue);
	void SetContents(bool bValue);
	void SetContents(float fValue);

	bool GetChildContents(char* sOutStr, const char* sChildTagName, int nMaxCharNum = -1);
	bool GetChildContents(int* iOutValue, const char* sChildTagName);
	bool GetChildContents(float* fOutValue, const char* sChildTagName);
	bool GetChildContents(bool* bOutValue, const char* sChildTagName);

	template<size_t size> bool GetAttribute(char(&sOutText)[size], const char *szAttrName,
		const char *sDefaultText = "") {
		return GetAttribute(sOutText, size, szAttrName, sDefaultText);
	}
	bool GetAttribute(char* sOutText, int maxlen, const char* sAttrName, const char* sDefaultText = "");
	bool GetAttribute(int* ipOutValue, const char* sAttrName, int nDefaultValue = 0);
	bool GetAttribute(bool* bOutValue, const char* sAttrName, bool bDefaultValue = false);
	bool GetAttribute(float* fpOutValue, const char* sAttrName, float fDefaultValue = 0.0f);
	bool GetAttribute(std::string* pstrOutValue, const char* sAttrName, const char* sDefaultValue = "");
	bool AddAttribute(const char* sAttrName, const char* sAttrText);
	bool AddAttribute(const char* sAttrName, int iAttrValue);
	bool AddAttribute(const char* sAttrName, bool bAttrValue);
	bool SetAttribute(const char* sAttrName, char* sAttrText);
	bool SetAttribute(const char* sAttrName, int iAttrValue);
	bool RemoveAttribute(const char* sAttrName);

	int GetAttributeCount();
	template<size_t size1, size_t size2> void GetAttribute(int index, char(&szoutAttrName)[size1], char(&szoutAttrValue)[size2]) {
		GetAttribute(index, szoutAttrName, size1, szoutAttrValue, size2);
	}
	void GetAttribute(int index, char* szoutAttrName, int maxlen1, char* szoutAttrValue, int maxlen2);

	bool AppendChild(const char* sTagName, const char* sTagText = NULL);
	bool AppendChild(MXmlElement aChildElement);

	MXmlElement	CreateChildElement(const char* sTagName);

	bool AppendText(const char* sText);
};

class MXmlDocument final
{
public:
	// Dummy methods for backwards compatibility.
	bool Create() { return true; }
	bool Destroy() { return true; }

	bool				LoadFromFile(const char* m_sFileName, MZFileSystem* FileSystem = nullptr);
	// If Size is -1, szBuffer must be null-terminated.
	bool				LoadFromMemory(char* szBuffer, size_t Size = -1);

	bool				SaveToFile(const char* m_sFileName);

	bool				CreateProcessingInstruction( const char* szHeader = "version=\"1.0\"");
	bool				Delete(MXmlNode* pNode);

	MXmlElement			CreateElement(const char* sName);

	bool				AppendChild(MXmlNode node);

	MXmlElement			GetDocumentElement() { return MXmlElement(Root); }

protected:
	// Parses the data in FileBuffer.
	bool Parse(const char* Filename = nullptr);

	// The RapidXML document.
	MXmlDomDoc Doc;

	// For some reason, all MAIET XMLs have an "<XML>" tag wrapped around their data, even though
	// MXml users expect not to see it.
	// So, this data member holds that element in order to give it out in GetDocumentElement.
	MXmlDomNodePtr Root{};

	// The memory in the file.
	// RapidXML doesn't save it, it only gives you back pointers into the memory you gave it, so we
	// need to save it ourselves.
	std::string FileBuffer;
};