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

#include "sst_config.h"
#include "sst/core/serialization/archive.h"

#include <iostream>
#include <string>
#include <fstream>

#include "sst/core/archive.h"
#include "sst/core/simulation.h"
#include "sst/core/sync.h"

using namespace SST;

Archive::Archive(std::string ttype, std::string filename) :
    filename(filename)
{
#if SST_WANT_POLYMORPHIC_ARCHIVE
    if (ttype.compare("xml") &&
        ttype.compare("text") &&
        ttype.compare("bin")) {
        fprintf(stderr, "Serialization type %s unknown.  Defaulting to bin\n",
                ttype.c_str());
        type = "bin";
    } else {
        type = ttype;
    }
#else
    if (ttype.compare("bin")) {
        fprintf(stderr, "Serialization type %s unknown.  Defaulting to bin\n",
                ttype.c_str());
    }
    type = "bin";
#endif
}


Archive::~Archive()
{
}


void
Archive::SaveSimulation(Simulation* simulation)
{
    std::string savename = filename + "." + type;
    std::ofstream ofs(savename.c_str());

#if SST_WANT_POLYMORPHIC_ARCHIVE
    if (type == "xml") {
        boost::archive::polymorphic_xml_oarchive oa(ofs);
        oa << BOOST_SERIALIZATION_NVP(simulation);
    } else if (type == "text") {
        boost::archive::polymorphic_text_oarchive oa(ofs);
        oa << BOOST_SERIALIZATION_NVP(simulation);
    } else if (type == "bin") {
        boost::archive::polymorphic_binary_oarchive oa(ofs);
        oa << BOOST_SERIALIZATION_NVP(simulation);
    } else {
        abort();
    }
#else
    if (type == "bin") {
        boost::archive::binary_oarchive oa(ofs);
        oa << BOOST_SERIALIZATION_NVP(simulation);
    } else {
        abort();
    }
#endif
}


Simulation* 
Archive::LoadSimulation(void)
{
    std::string loadname = filename + "." + type;
    std::ifstream ifs(loadname.c_str());
    Simulation* simulation;

#if SST_WANT_POLYMORPHIC_ARCHIVE
    if (type == "xml") {
        boost::archive::polymorphic_xml_iarchive ia(ifs);
        ia >> BOOST_SERIALIZATION_NVP(simulation);
    } else if (type == "text") {
        boost::archive::polymorphic_text_iarchive ia(ifs);
        ia >> BOOST_SERIALIZATION_NVP(simulation);
    } else if (type == "bin") {
        boost::archive::polymorphic_binary_iarchive ia(ifs);
        ia >> BOOST_SERIALIZATION_NVP(simulation);
    } else {
        abort();
    }
#else
    if (type == "bin") {
        boost::archive::binary_iarchive ia(ifs);
        ia >> BOOST_SERIALIZATION_NVP(simulation);
    } else {
        abort();
    }
#endif

    return simulation;
}
