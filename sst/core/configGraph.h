// -*- c++ -*-

// Copyright 2009-2013 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
// 
// Copyright (c) 2009-2013, Sandia Corporation
// All rights reserved.
// 
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#ifndef SST_CORE_CONFIGGRAPH_H
#define SST_CORE_CONFIGGRAPH_H

#include "sst/core/sst_types.h"
#include <sst/core/serialization.h>

#include <vector>
#include <map>

#include "sst/core/graph.h"
#include "sst/core/params.h"

namespace SST {

class Simulation;

class Config;
class ConfigLink;

/** Represents the configuration of a generic component */
class ConfigComponent {
public:
    ComponentId_t            id;                /*!< Unique ID of this component */
    std::string              name;              /*!< Name of this component */
    std::string              type;              /*!< Type of this component */
    float                    weight;            /*!< Parititoning weight for this component */
    int                      rank;              /*!< Parallel Rank for this component */
    std::vector<ConfigLink*> links;             /*!< List of links connected */
    Params                   params;            /*!< Set of Parameters */
    bool                     isIntrospector;    /*!< Is this an Introspector? */

    /** Create a new Component */
    ConfigComponent(std::string name, std::string type, float weight, int rank, bool isIntrospector) :
        name(name),
        type(type),
        weight(weight),
        rank(rank),
        isIntrospector(isIntrospector)
    {
        id = count++;
    }

    ConfigComponent() :
        isIntrospector(false)
    {
        id = count++;
    }

    /** Print Component information */
    void print(std::ostream &os) const;

    /** Generate Dot information for this Component */
	void genDot(std::ostream &os) const;

private:

    friend class ConfigGraph;
    
    static ComponentId_t count;

    friend class boost::serialization::access;
    template<class Archive>
    void
    serialize(Archive & ar, const unsigned int version )
    {
	ar & BOOST_SERIALIZATION_NVP(id);
	ar & BOOST_SERIALIZATION_NVP(name);
	ar & BOOST_SERIALIZATION_NVP(type);
	ar & BOOST_SERIALIZATION_NVP(weight);
	ar & BOOST_SERIALIZATION_NVP(rank);
	ar & BOOST_SERIALIZATION_NVP(links);
	ar & BOOST_SERIALIZATION_NVP(params);
	ar & BOOST_SERIALIZATION_NVP(isIntrospector);
    }


};

/** Represents the configuration of a generic Link */
class ConfigLink {
public:
    LinkId_t         id;            /*!< ID of this link */
    std::string      name;          /*!< Name of this link */
    ComponentId_t    component[2];  /*!< IDs of the connected components */
    std::string      port[2];       /*!< Names of the connected ports */
    SimTime_t        latency[2];    /*!< Latency from each side */
    int              current_ref;   /*!< Number of components currently referring to this Link */

    ConfigLink()
    {
        id = count++;
        current_ref = 0;

        // Initialize the component data items
        component[0] = ULONG_MAX;
        component[1] = ULONG_MAX;
    }


    /** Return the minimum latency of this link (from both sides) */
    SimTime_t getMinLatency() {
        if ( latency[0] < latency[1] ) return latency[0];
        return latency[1];
    }

    /** Print the Link information */
    void print(std::ostream &os) const {
        os << "Link " << name << " (id = " << id << ")" << std::endl;
        os << "  component[0] = " << component[0] << std::endl;
        os << "  port[0] = " << port[0] << std::endl;
        os << "  latency[0] = " << latency[0] << std::endl;
        os << "  component[1] = " << component[1] << std::endl;
        os << "  port[1] = " << port[1] << std::endl;
        os << "  latency[1] = " << latency[1] << std::endl;
    }

    /** Generate Dot-style Link information */
	void genDot(std::ostream &os) const {
		os << component[0] << ":\"" << port[0] << "\""
            << " -- " << component[1] << ":" << port[1]
            << " [label=\"" << name << "\"] ;\n";
	}


private:
    static LinkId_t count;

    friend class boost::serialization::access;
    template<class Archive>
    void
    serialize(Archive & ar, const unsigned int version )
    {
	ar & BOOST_SERIALIZATION_NVP(id);
	ar & BOOST_SERIALIZATION_NVP(name);
	ar & BOOST_SERIALIZATION_NVP(component);
	ar & BOOST_SERIALIZATION_NVP(port);
	ar & BOOST_SERIALIZATION_NVP(latency);
	ar & BOOST_SERIALIZATION_NVP(current_ref);
    }


};


/** Map names to Links */
typedef std::map<std::string,ConfigLink*> ConfigLinkMap_t;
/** Map IDs to Components */
typedef std::map<ComponentId_t,ConfigComponent*> ConfigComponentMap_t;
/** Map names to Parameter Sets: XML only */
typedef std::map<std::string,Params*> ParamsMap_t;
/** Map names to variable values:  XML only */
typedef std::map<std::string,std::string> VariableMap_t;


/** A Configuration Graph
 *  A graph representing Components and Links
 */
class ConfigGraph {
public:
    /** Print the configuration graph */
	void print(std::ostream &os) const {
		os << "Printing graph" << std::endl;
		for (ConfigComponentMap_t::const_iterator i = comps.begin() ; i != comps.end() ; ++i) {
			(*i).second->print(os);
		}
	}

    /** Generate Dot-style output of the configuration graph */
	void genDot(std::ostream &os, const std::string &name) const;

    /** Helper function to set all the ranks to the same value */
    void setComponentRanks(int rank);
    /** Checks to see if rank contains at least one component */
    bool containsComponentInRank(int rank);
    /** Verify that all components have valid Ranks assigned */
    bool checkRanks(int ranks);


    // API for programatic initialization
    /** Create a new component with weight and rank */
    ComponentId_t addComponent(std::string name, std::string type, float weight, int rank);
    /** Create a new component */
    ComponentId_t addComponent(std::string name, std::string type);

    /** Set on which rank a Component will exist (partitioning) */
    void setComponentRank(ComponentId_t comp_id, int rank);
    /** Set the weight of a Component (partitioning) */
    void setComponentWeight(ComponentId_t comp_id, float weight);

    /** Add a set of Parameters to a component */
    void addParams(ComponentId_t comp_id, Params& p);
    /** Add a single parameter to a component */
    void addParameter(ComponentId_t comp_id, std::string key, std::string value, bool overwrite = false);

    /** Add a Link to a Component on a given Port */
    void addLink(ComponentId_t comp_id, std::string link_name, std::string port, std::string latency_str);

    /** Create a new Introspector */
    ComponentId_t addIntrospector(std::string name, std::string type);

    /** Check the graph for Structural errors */
    bool checkForStructuralErrors();

    /** Dump the configuration to a file
     * @param filePath - Name of File to write
     * @param cfg - Configuration options
     * @param asDot - if True, write as a Dot-style graph
     */
    void dumpToFile(std::string filePath, Config* cfg, bool asDot);
    /** Escape names to be safe to parse in Python */
    std::string makeNamePythonSafe(const std::string name);
    /** Escape to make safe, strings */
    static std::string escapeString(const std::string value);

    // Temporary until we have a better API
    /** Return the map of components */
    ConfigComponentMap_t& getComponentMap() {
        return comps;
    }
    /** Return the map of links */
    ConfigLinkMap_t& getLinkMap() {
        return links;
    }

private:
    friend class Simulation;
    friend class SSTSDLModelDefinition;

    ConfigLinkMap_t      links;
    ConfigComponentMap_t comps;


    friend class boost::serialization::access;
    template<class Archive>
    void
    serialize(Archive & ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_NVP(links);
		ar & BOOST_SERIALIZATION_NVP(comps);
	}


};

 
} // namespace SST

#endif // SST_CORE_CONFIGGRAPH_H
