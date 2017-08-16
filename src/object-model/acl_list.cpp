/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#include "acl_list.hpp"
#include "logger.hpp"

namespace VPP
{
    namespace ACL
    {
        template <> void l2_list::event_handler::handle_populate(const client_db::key_t &key)
        {
            /* hack to get this function instantiated */
            m_evh.order();

            /*
             * dump VPP Bridge domains
             */
            std::shared_ptr<l2_list::dump_cmd> cmd(new l2_list::dump_cmd());

            HW::enqueue(cmd);
            HW::write();

            for (auto &record : *cmd)
            {
                auto &payload = record.get_payload();

                const handle_t hdl(payload.acl_index);
                l2_list acl(hdl, std::string(reinterpret_cast<const char*>(payload.tag)));

                for (unsigned int ii = 0; ii < payload.count; ii++)
                {
                    const route::prefix_t pfx(payload.r[0].is_ipv6,
                                              payload.r[0].src_ip_addr,
                                              payload.r[0].src_ip_prefix_len);
                    l2_rule rule(ii,
                                action_t::from_int(payload.r[0].is_permit),
                                pfx,
                                {payload.r[0].src_mac},
                                {payload.r[0].src_mac_mask});

                    acl.insert(rule);
                }
                BOOST_LOG_SEV(logger(), levels::debug) << "dump: " << acl.to_string();

                /*
                 * Write each of the discovered ACLs into the OM,
                 * but disable the HW Command q whilst we do, so that no
                 * commands are sent to VPP
                 */
                VPP::OM::commit(key, acl);
            }
        }

        template <> void l3_list::event_handler::handle_populate(const client_db::key_t &key)
        {
        }
    };
};
