/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#include <iostream>

#include "interface.hpp"

using namespace VPP;

/*
 * constants and enums
 */
const interface::type_t interface::type_t::UNKNOWN(0, "unknown");
const interface::type_t interface::type_t::BVI(1, "BVI");
const interface::type_t interface::type_t::ETHERNET(2, "Ehternet");
const interface::type_t interface::type_t::VXLAN(3, "VXLAN");
const interface::type_t interface::type_t::AFPACKET(4, "AFPACKET");
const interface::type_t interface::type_t::LOOPBACK(5, "LOOPBACK");
const interface::type_t interface::type_t::LOCAL(6, "LOCAL");
const interface::type_t interface::type_t::TAP(7, "TAP");

const interface::oper_state_t interface::oper_state_t::DOWN(0, "down");
const interface::oper_state_t interface::oper_state_t::UP(1, "up");

const interface::admin_state_t interface::admin_state_t::DOWN(0, "down");
const interface::admin_state_t interface::admin_state_t::UP(1, "up");

interface::type_t interface::type_t::from_string(const std::string &str)
{
    if (str.find("Ethernet") != std::string::npos)
    {
        return interface::type_t::ETHERNET;
    }
    else if (str.find("vxlan") != std::string::npos)
    {
        return interface::type_t::VXLAN;
    }
    else if (str.find("loop") != std::string::npos)
    {
        return interface::type_t::LOOPBACK;
    }
    else if (str.find("host-") != std::string::npos)
    {
        return interface::type_t::AFPACKET;
    }
    else if (str.find("local") != std::string::npos)
    {
        return interface::type_t::LOCAL;
    }
    else if (str.find("tap") != std::string::npos)
    {
        return interface::type_t::TAP;
    }
    else if (str.find("bvi") != std::string::npos)
    {
        return interface::type_t::BVI;
    }

    return interface::type_t::UNKNOWN;
}

interface::type_t::type_t(int v, const std::string &s):
    enum_base<interface::type_t>(v, s)
{
}

interface::oper_state_t::oper_state_t(int v, const std::string &s):
    enum_base<interface::oper_state_t>(v, s)
{
}

interface::admin_state_t::admin_state_t(int v, const std::string &s):
    enum_base<interface::admin_state_t>(v, s)
{
}

interface::admin_state_t interface::admin_state_t::from_int(uint8_t v)
{
    if (0 == v)
    {
        return (interface::admin_state_t::DOWN);
    }
    return (interface::admin_state_t::UP);
}

interface::oper_state_t interface::oper_state_t::from_int(uint8_t v)
{
    if (0 == v)
    {
        return (interface::oper_state_t::DOWN);
    }
    return (interface::oper_state_t::UP);
}
