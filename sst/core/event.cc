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
#include "sst/core/serialization.h"
#include "sst/core/event.h"

#include "sst/core/link.h"

namespace SST {

Event::~Event() {}

    
void    
Event:: execute(void)
{
    delivery_link->deliverEvent(this);
}

    
template<class Archive>
void
Event::serialize(Archive & ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Activity);
    // ar & BOOST_SERIALIZATION_NVP(delivery_link);
    ar & BOOST_SERIALIZATION_NVP(link_id);
}

void
NullEvent::execute(void)
{
    delivery_link->deliverEvent(NULL);
    delete this;
}

    
template<class Archive>
void
NullEvent::serialize(Archive & ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Event);
}


#ifdef USE_MEMPOOL
std::vector<std::pair<size_t, Core::MemPool*> > Activity::memPools;
#endif

} // namespace SST


SST_BOOST_SERIALIZATION_INSTANTIATE(SST::Event::serialize)
SST_BOOST_SERIALIZATION_INSTANTIATE(SST::NullEvent::serialize)

BOOST_CLASS_EXPORT_IMPLEMENT(SST::Event)
BOOST_CLASS_EXPORT_IMPLEMENT(SST::NullEvent)
