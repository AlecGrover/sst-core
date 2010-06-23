// Copyright 2009-2010 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
// 
// Copyright (c) 2009-2010, Sandia Corporation
// All rights reserved.
// 
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#ifndef SST_CORE_SERIALIZATION_ELEMENT_H
#define SST_CORE_SERIALIZATION_ELEMENT_H

#if defined(SST_CORE_SERIALIZATION_ARCHIVE_H) || defined(SST_CORE_SERIALIZATION_CORE_H)
#error "Include only one serialization/ header file"
#endif

#if SST_WANT_POLYMORPHIC_ARCHIVE
#include <boost/archive/polymorphic_iarchive.hpp>
#include <boost/archive/polymorphic_oarchive.hpp>
#else
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/mpi.hpp>

#include "sst/core/serialization/types.h"

#endif // #ifndef SST_CORE_SERIALIZATION_ELEMENT_H
