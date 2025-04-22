/*
 * Copyright (C) 2025 ZNC MultiJoin Module
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <znc/IRCNetwork.h>
#include <znc/Client.h>
#include <znc/Chan.h>
#include <znc/User.h>
#include <znc/IRCSock.h>
#include <znc/Modules.h>
#include <znc/znc.h>
#include <set>

class CMultiJoinMod : public CModule {
public:
    MODCONSTRUCTOR(CMultiJoinMod) {
        AddHelpCommand();
        AddCommand("join", t_d("<#channel> [network_name]"), t_d("Joins all your nicks on the specified network to the channel"), 
            [=](const CString& sLine) { JoinCommand(sLine); });
        AddCommand("part", t_d("<#channel> [network_name]"), t_d("Parts all your nicks on the specified network from the channel"), 
            [=](const CString& sLine) { PartCommand(sLine); });
        AddCommand("list", t_d("[network_name]"), t_d("Lists all nicks on the specified network"), 
            [=](const CString& sLine) { ListCommand(sLine); });
    }
    
    virtual ~CMultiJoinMod() {
    }
    
    bool OnLoad(const CString& sArgs, CString& sMessage) override {
        sMessage = t_s("MultiJoin module loaded. Usage: /msg *multijoin help");
        return true;
    }
    
    void JoinCommand(const CString& sLine) {
        CString sChannel = sLine.Token(1);
        CString sNetwork = sLine.Token(2);
        
        if (sChannel.empty()) {
            PutModule(t_s("Usage: join #channel [network_name]"));
            return;
        }
        
        if (!sChannel.StartsWith("#")) {
            sChannel = "#" + sChannel;
        }
        
        JoinAllToChannel(sChannel, sNetwork);
    }
    
    void PartCommand(const CString& sLine) {
        CString sChannel = sLine.Token(1);
        CString sNetwork = sLine.Token(2);
        
        if (sChannel.empty()) {
            PutModule(t_s("Usage: part #channel [network_name]"));
            return;
        }
        
        if (!sChannel.StartsWith("#")) {
            sChannel = "#" + sChannel;
        }
        
        PartAllFromChannel(sChannel, sNetwork);
    }
    
    void ListCommand(const CString& sLine) {
        CString sNetwork = sLine.Token(1);
        ListNetworkClients(sNetwork);
    }
    
    void ListNetworkClients(const CString& sTargetNetwork) {
        CString sCurrentNetwork = GetNetwork()->GetName();
        CString sNetworkToList = sTargetNetwork.empty() ? sCurrentNetwork : sTargetNetwork;
        CString sLowerNetwork = sNetworkToList.AsLower();
        
        PutModule(t_f("All nicks on network {1}:")(sNetworkToList));
        
        unsigned int uFoundCount = 0;
        
        const std::map<CString, CUser*>& mUsers = CZNC::Get().GetUserMap();
        for (const auto& it : mUsers) {
            CUser* pUser = it.second;
            
            for (CIRCNetwork* pNetwork : pUser->GetNetworks()) {
                if (pNetwork->GetName().AsLower() == sLowerNetwork) {
                    if (pNetwork->IsIRCConnected()) {
                        PutModule(t_f("  - {1} ({2}) [active]")(
                            pNetwork->GetIRCSock()->GetNick(), 
                            pUser->GetUsername()));
                    } else {
                        PutModule(t_f("  - {1} ({2}) [not connected]")(
                            pNetwork->GetNick(), 
                            pUser->GetUsername()));
                    }
                    uFoundCount++;
                }
            }
        }
        
        if (uFoundCount == 0) {
            PutModule(t_f("No network found with the name {1}, or no nicks are connected on it.")(sNetworkToList));
        } else {
            PutModule(t_f("Total of {1} nick(s) found.")(uFoundCount));
        }
    }
    
    void JoinAllToChannel(const CString& sChannel, const CString& sTargetNetwork) {
        CString sCurrentNetwork = GetNetwork()->GetName();
        CString sNetworkToUse = sTargetNetwork.empty() ? sCurrentNetwork : sTargetNetwork;
        CString sLowerNetwork = sNetworkToUse.AsLower();
        
        unsigned int uJoinedCount = 0;
        
        const std::map<CString, CUser*>& mUsers = CZNC::Get().GetUserMap();
        for (const auto& it : mUsers) {
            CUser* pUser = it.second;
            
            for (CIRCNetwork* pNetwork : pUser->GetNetworks()) {
                if (pNetwork->GetName().AsLower() == sLowerNetwork) {
                    if (pNetwork->IsIRCConnected()) {
                        PutModule(t_f("Sending join command to channel {3} for nick {1} ({2})...")(
                            pNetwork->GetIRCSock()->GetNick(), 
                            pUser->GetUsername(), 
                            sChannel));
                        pNetwork->PutIRC("JOIN " + sChannel);
                        uJoinedCount++;
                    } else {
                        PutModule(t_f("Nick {1} ({2}) is not connected to IRC server, cannot join channel.")(
                            pNetwork->GetNick(), 
                            pUser->GetUsername()));
                    }
                }
            }
        }
        
        if (uJoinedCount > 0) {
            PutModule(t_f("Total of {2} nick(s) on network {1} have been instructed to join channel {3}.")(
                sNetworkToUse, uJoinedCount, sChannel));
        } else {
            PutModule(t_f("No network found with the name {1}, or no connected nicks on it.")(sNetworkToUse));
        }
    }
    
    void PartAllFromChannel(const CString& sChannel, const CString& sTargetNetwork) {
        CString sCurrentNetwork = GetNetwork()->GetName();
        CString sNetworkToUse = sTargetNetwork.empty() ? sCurrentNetwork : sTargetNetwork;
        CString sLowerNetwork = sNetworkToUse.AsLower();
        
        unsigned int uPartedCount = 0;
        
        const std::map<CString, CUser*>& mUsers = CZNC::Get().GetUserMap();
        for (const auto& it : mUsers) {
            CUser* pUser = it.second;
            
            for (CIRCNetwork* pNetwork : pUser->GetNetworks()) {
                if (pNetwork->GetName().AsLower() == sLowerNetwork) {
                    if (pNetwork->IsIRCConnected()) {
                        PutModule(t_f("Sending part command from channel {3} for nick {1} ({2})...")(
                            pNetwork->GetIRCSock()->GetNick(), 
                            pUser->GetUsername(), 
                            sChannel));
                        pNetwork->PutIRC("PART " + sChannel + " :Left via MultiJoin module");
                        uPartedCount++;
                    } else {
                        PutModule(t_f("Nick {1} ({2}) is not connected to IRC server, cannot part from channel.")(
                            pNetwork->GetNick(), 
                            pUser->GetUsername()));
                    }
                }
            }
        }
        
        if (uPartedCount > 0) {
            PutModule(t_f("Total of {2} nick(s) on network {1} have been instructed to part from channel {3}.")(
                sNetworkToUse, uPartedCount, sChannel));
        } else {
            PutModule(t_f("No network found with the name {1}, or no connected nicks on it.")(sNetworkToUse));
        }
    }
};

MODULEDEFS(CMultiJoinMod, "Join or part ALL your nicks on a given network to/from a channel with a single command")
