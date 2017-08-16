/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#include <cassert>
#include <iostream>

#include "interface.hpp"
#include "l2_binding.hpp"
#include "bridge_domain.hpp"
#include "cmd.hpp"
#include "logger.hpp"

using namespace VPP;

/**
 * A DB of al the interfaces, key on the name
 */
singular_db<uint32_t, bridge_domain> bridge_domain::m_db;

bridge_domain::event_handler bridge_domain::m_evh;

/**
 * Construct a new object matching the desried state
 */
bridge_domain::bridge_domain(uint32_t id):
    m_id(id)
{
}

bridge_domain::bridge_domain(const bridge_domain& o):
    m_id(o.m_id)
{
}

uint32_t bridge_domain::id() const
{
    return (m_id.data());
}

void bridge_domain::sweep()
{
    if (rc_t::OK == m_id.rc())
    {
        HW::enqueue(new delete_cmd(m_id));
    }
    HW::write();
}

void bridge_domain::replay()
{
    if (rc_t::OK == m_id.rc())
    {
        HW::enqueue(new create_cmd(m_id));
    }
}

bridge_domain::~bridge_domain()
{
    sweep();

    // not in the DB anymore.
    m_db.release(m_id.data(), this);
}

std::string bridge_domain::to_string() const
{
    std::ostringstream s;
    s << "bridge-domain:["
      << m_id.to_string()
      << "]";

    return (s.str());
}

void bridge_domain::update(const bridge_domain &desired)
{
    /*
     * the desired state is always that the interface should be created
     */
    if (rc_t::OK != m_id.rc())
    {
        HW::enqueue(new create_cmd(m_id));
    }
}

std::shared_ptr<bridge_domain> bridge_domain::find_or_add(const bridge_domain &temp)
{
    return (m_db.find_or_add(temp.m_id.data(), temp));
}

std::shared_ptr<bridge_domain> bridge_domain::singular() const
{
    return find_or_add(*this);
}

void bridge_domain::dump(std::ostream &os)
{
    m_db.dump(os);
}

void bridge_domain::event_handler::handle_populate(const client_db::key_t &key)
{
    /*
     * dump VPP Bridge domains
     */
    std::shared_ptr<bridge_domain::dump_cmd> cmd(new bridge_domain::dump_cmd());

    HW::enqueue(cmd);
    HW::write();

    for (auto &record: *cmd)
    {
        auto &payload = record.get_payload();

        bridge_domain bd(payload.bd_id);

        BOOST_LOG_SEV(logger(), levels::debug) << "dump: " << bd.to_string();

        /*
         * Write each of the discovered interfaces into the OM,
         * but disable the HW Command q whilst we do, so that no
         * commands are sent to VPP
         */
        VPP::OM::commit(key, bd);

        /**
         * For each interface in the BD construct an l2_binding
         */
        for (unsigned int ii = 0; ii < payload.n_sw_ifs; ii++)
        {
            std::shared_ptr<interface> itf =
                interface::find(payload.sw_if_details[ii].sw_if_index);
            l2_binding l2(*itf, bd);
            OM::commit(key, l2);
        }
    }
}

bridge_domain::event_handler::event_handler()
{
    OM::register_listener(this);
    inspect::register_handler({"bd", "bridge"}, "Bridge Domains", this);
}

void bridge_domain::event_handler::handle_replay()
{
    m_db.replay();
}

dependency_t bridge_domain::event_handler::order() const
{
    return (dependency_t::FORWARDING_DOMAIN);
}

void bridge_domain::event_handler::show(std::ostream &os)
{
    m_db.dump(os);
}