/******************************************************************************
* Copyright (c) 2014, Michael P. Gerlek (mpg@flaxen.com)
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#include <pdal/PDALUtils.hpp>

using namespace std;

namespace pdal
{

namespace {

void toJSON(const MetadataNode& m, std::ostream& o, int level);
void arrayToJSON(const MetadataNodeList& children, std::ostream& o, int level);
void arrayEltToJSON(const MetadataNode& m, std::ostream& o, int level);
void subnodesToJSON(const MetadataNode& parent, std::ostream& o, int level)
{
    const std::string indent(level * 2, ' ');

    std::vector<std::string> names = parent.childNames();

    o << indent << "{" << endl;
    for (auto ni = names.begin(); ni != names.end(); ++ni)
    {
        MetadataNodeList children = parent.children(*ni);

        MetadataNode& node = *children.begin();
        if (node.kind() == MetadataType::Array)
        {
            o << indent << "  \"" << node.name() << "\":" << std::endl;
            arrayToJSON(children, o, level + 1);
        }
        else
            toJSON(node, o, level + 1);
        if (ni != names.rbegin().base() - 1)
            o << ",";
        o << std::endl;
    }
    o << indent << "}";
}

void arrayToJSON(const MetadataNodeList& children, std::ostream& o, int level)
{
    const std::string indent(level * 2, ' ');

    o << indent << "[" << std::endl;
    for (auto ci = children.begin(); ci != children.end(); ++ci)
    {
        const MetadataNode& m = *ci;

        arrayEltToJSON(m, o, level + 1);
        if (ci != children.rbegin().base() - 1)
            o << ",";
        o << std::endl;
    }
    o << indent << "]";
}

void arrayEltToJSON(const MetadataNode& m, std::ostream& o, int level)
{
    std::string indent(level * 2, ' ');
    std::string value = m.jsonValue();
    bool children = m.hasChildren();

    // This is a case from XML.  In JSON, you can't have two values.
    if (!value.empty() && children)
    {
        o << value << "," << std::endl;
        subnodesToJSON(m, o, level);
    }
    else if (!value.empty())
        o << indent << value;
    else
        subnodesToJSON(m, o, level);
    // There is the case where we have a name and no value to handle.  What
    // should be done?
}

void toJSON(const MetadataNode& m, std::ostream& o, int level)
{
    std::string indent(level * 2, ' ');
    std::string name = m.name();
    std::string value = m.jsonValue();
    bool children = m.hasChildren();

    if (name.empty())
        name = "unnamed";

    // This is a case from XML.  In JSON, you can't have two values.
    if (!value.empty() && children)
    {
        o << indent << "\"" << name << "\": " << value << "," << std::endl;
        o << indent << "\"" << name << "\": ";
        subnodesToJSON(m, o, level);
    }
    else if (!value.empty())
        o << indent << "\"" << name << "\": " << value;
    else
    {
        o << indent << "\"" << name << "\":" << std::endl;
        subnodesToJSON(m, o, level);
    }
    // There is the case where we have a name and no value to handle.  What
    // should be done?
}


} // unnamed namespace

namespace utils
{

std::string toJSON(const MetadataNode& m)
{
    std::ostringstream o;

    toJSON(m, o);
    return o.str();
}

void toJSON(const MetadataNode& m, std::ostream& o)
{
    if (m.name().empty())
        pdal::subnodesToJSON(m, o, 0);
    else if (m.kind() == MetadataType::Array)
        pdal::arrayToJSON(m.children(), o, 0);
    else
    {
        o << "{" << std::endl;
        pdal::toJSON(m, o, 1);
        o << std::endl;
        o << "}";
    }
    o << std::endl;
}

namespace reST
{
    
using namespace boost::property_tree;

static std::string indent(int level)
{
    std::string s;
    for (int i=0; i<level; i++) s += "    ";
    return s;
}


void write_rst(std::ostream& ost,
               const boost::property_tree::ptree& pt,
               int level)
{
    using boost::property_tree::ptree;

    if (pt.empty())
    {
        ost << pt.data();
        ost << endl << endl;
    }
    else
    {
        if (level) ost << endl << endl;
        for (ptree::const_iterator pos = pt.begin(); pos != pt.end();)
        {
            ost << indent(level+1) << "- " << pos->first << ": ";
            write_rst(ost, pos->second, level + 1);
            ++pos;
            //ost << endl << endl;
        }
    }
}


std::ostream& toRST(const ptree& pt, std::ostream& os)
{
    write_rst(os, pt);
    return os;
}

} // namespace reST
} // namespace utils
} // namespace pdal
