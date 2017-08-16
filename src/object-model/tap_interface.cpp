/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#include <typeinfo>
#include <cassert>
#include <iostream>

#include "cmd.hpp"
#include "tap_interface.hpp"

#include <vpp/vpe.api.vapi.hpp>

using namespace VPP;

/**
 * Construct a new object matching the desried state
 */
tap_interface::tap_interface(const std::string &name,
                             admin_state_t state,
                             route::prefix_t prefix):
    interface(name, type_t::TAP, state),
    m_prefix(prefix),
    m_l2_address(l2_address_t::ZERO)
{
}

tap_interface::tap_interface(const std::string &name,
                             admin_state_t state,
                             route::prefix_t prefix,
                             const l2_address_t &l2_address):
    interface(name, type_t::TAP, state),
    m_prefix(prefix),
    m_l2_address(l2_address)
{
}

tap_interface::tap_interface(const handle_t &hdl,
                             const std::string &name,
                             admin_state_t state,
                             route::prefix_t prefix):
    interface(hdl, l2_address_t::ZERO, name, type_t::TAP, state),
    m_prefix(prefix),
    m_l2_address(l2_address_t::ZERO)
{
}

tap_interface::~tap_interface()
{
    sweep();
    release();
}

tap_interface::tap_interface(const tap_interface& o):
    interface(o),
    m_prefix(o.m_prefix),
    m_l2_address(o.m_l2_address)
{
}

std::queue<cmd*> &  tap_interface::mk_create_cmd(std::queue<cmd*> &q)
{
    q.push(new create_cmd(m_hdl, name(), m_prefix, m_l2_address));

    return (q);
}

std::queue<cmd*> &  tap_interface::mk_delete_cmd(std::queue<cmd*> &q)
{
    q.push(new delete_cmd(m_hdl));

    return (q);
}

std::shared_ptr<tap_interface> tap_interface::singular() const
{
    return std::dynamic_pointer_cast<tap_interface>(singular_i());
}

std::shared_ptr<interface> tap_interface::singular_i() const
{
    return m_db.find_or_add(name(), *this);
}

tap_interface::create_cmd::create_cmd(HW::item<handle_t> &item,
                                      const std::string &name,
                                      route::prefix_t &prefix,
                                      const l2_address_t &l2_address):
    interface::create_cmd<vapi::Tap_connect>(item, name),
    m_prefix(prefix),
    m_l2_address(l2_address)
{
}

rc_t tap_interface::create_cmd::issue(connection &con)
{
    msg_t req(con.ctx(), std::ref(*this));

    auto &payload = req.get_request().get_payload();
    memset(payload.tap_name, 0,
           sizeof(payload.tap_name));
    memcpy(payload.tap_name, m_name.c_str(),
           std::min(m_name.length(),
                    sizeof(payload.tap_name)));
    if (m_prefix != route::prefix_t::ZERO) {
        if (m_prefix.address().is_v6()) {
            m_prefix.to_vpp(&payload.ip6_address_set,
                            payload.ip6_address,
                            &payload.ip6_mask_width);
        } else {
            m_prefix.to_vpp(&payload.ip4_address_set,
                            payload.ip4_address,
                            &payload.ip4_mask_width);
            payload.ip4_address_set = 1;
        }
    }

    if (m_l2_address != l2_address_t::ZERO) {
        m_l2_address.to_bytes(payload.mac_address, 6);
    }

    VAPI_CALL(req.execute());

    m_hw_item =  wait();

    return rc_t::OK;
}

std::string tap_interface::create_cmd::to_string() const
{
    std::ostringstream s;
    s << "tap-intf-create: " << m_hw_item.to_string()
      << " ip-prefix:" << m_prefix.to_string(); 

    return (s.str());
}

tap_interface::delete_cmd::delete_cmd(HW::item<handle_t> &item):
    interface::delete_cmd<vapi::Tap_delete>(item)
{
}

rc_t tap_interface::delete_cmd::issue(connection &con)
{
    // finally... call VPP

    return rc_t::OK;
}
std::string tap_interface::delete_cmd::to_string() const
{
    std::ostringstream s;
    s << "tap-itf-delete: " << m_hw_item.to_string();

    return (s.str());
}
