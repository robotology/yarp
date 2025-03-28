/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <IOdometry2DMsgs.h>

#include <yarp/os/idl/WireTypes.h>

#include <algorithm>

// reset_odometry_RPC helper class declaration
class IOdometry2DMsgs_reset_odometry_RPC_helper :
        public yarp::os::Portable
{
public:
    IOdometry2DMsgs_reset_odometry_RPC_helper() = default;
    bool write(yarp::os::ConnectionWriter& connection) const override;
    bool read(yarp::os::ConnectionReader& connection) override;

    class Command :
            public yarp::os::idl::WirePortable
    {
    public:
        Command() = default;
        ~Command() override = default;

        bool write(yarp::os::ConnectionWriter& connection) const override;
        bool read(yarp::os::ConnectionReader& connection) override;

        bool write(const yarp::os::idl::WireWriter& writer) const override;
        bool writeTag(const yarp::os::idl::WireWriter& writer) const;
        bool writeArgs(const yarp::os::idl::WireWriter& writer) const;

        bool read(yarp::os::idl::WireReader& reader) override;
        bool readTag(yarp::os::idl::WireReader& reader);
        bool readArgs(yarp::os::idl::WireReader& reader);
    };

    class Reply :
            public yarp::os::idl::WirePortable
    {
    public:
        Reply() = default;
        ~Reply() override = default;

        bool write(yarp::os::ConnectionWriter& connection) const override;
        bool read(yarp::os::ConnectionReader& connection) override;

        bool write(const yarp::os::idl::WireWriter& writer) const override;
        bool read(yarp::os::idl::WireReader& reader) override;

        yarp::dev::ReturnValue return_helper{};
    };

    using funcptr_t = yarp::dev::ReturnValue (*)();
    void call(IOdometry2DMsgs* ptr);

    Command cmd;
    Reply reply;

    static constexpr const char* s_tag{"reset_odometry_RPC"};
    static constexpr size_t s_tag_len{3};
    static constexpr size_t s_cmd_len{3};
    static constexpr size_t s_reply_len{1};
    static constexpr const char* s_prototype{"yarp::dev::ReturnValue IOdometry2DMsgs::reset_odometry_RPC()"};
    static constexpr const char* s_help{""};
};

// reset_odometry_RPC helper class implementation
bool IOdometry2DMsgs_reset_odometry_RPC_helper::write(yarp::os::ConnectionWriter& connection) const
{
    return cmd.write(connection);
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::read(yarp::os::ConnectionReader& connection)
{
    return reply.read(connection);
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Command::write(yarp::os::ConnectionWriter& connection) const
{
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(s_cmd_len)) {
        return false;
    }
    return write(writer);
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Command::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader()) {
        reader.fail();
        return false;
    }
    return read(reader);
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Command::write(const yarp::os::idl::WireWriter& writer) const
{
    if (!writeTag(writer)) {
        return false;
    }
    if (!writeArgs(writer)) {
        return false;
    }
    return true;
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Command::writeTag(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeTag(s_tag, 1, s_tag_len)) {
        return false;
    }
    return true;
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Command::writeArgs(const yarp::os::idl::WireWriter& writer [[maybe_unused]]) const
{
    return true;
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Command::read(yarp::os::idl::WireReader& reader)
{
    if (!readTag(reader)) {
        return false;
    }
    if (!readArgs(reader)) {
        return false;
    }
    return true;
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Command::readTag(yarp::os::idl::WireReader& reader)
{
    std::string tag = reader.readTag(s_tag_len);
    if (reader.isError()) {
        return false;
    }
    if (tag != s_tag) {
        reader.fail();
        return false;
    }
    return true;
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Command::readArgs(yarp::os::idl::WireReader& reader)
{
    if (!reader.noMore()) {
        reader.fail();
        return false;
    }
    return true;
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Reply::write(yarp::os::ConnectionWriter& connection) const
{
    yarp::os::idl::WireWriter writer(connection);
    return write(writer);
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Reply::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::idl::WireReader reader(connection);
    return read(reader);
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Reply::write(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.isNull()) {
        if (!writer.write(return_helper)) {
            return false;
        }
    }
    return true;
}

bool IOdometry2DMsgs_reset_odometry_RPC_helper::Reply::read(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.read(return_helper)) {
        reader.fail();
        return false;
    }
    return true;
}

void IOdometry2DMsgs_reset_odometry_RPC_helper::call(IOdometry2DMsgs* ptr)
{
    reply.return_helper = ptr->reset_odometry_RPC();
}

// Constructor
IOdometry2DMsgs::IOdometry2DMsgs()
{
    yarp().setOwner(*this);
}

yarp::dev::ReturnValue IOdometry2DMsgs::reset_odometry_RPC()
{
    if (!yarp().canWrite()) {
        yError("Missing server method '%s'?", IOdometry2DMsgs_reset_odometry_RPC_helper::s_prototype);
    }
    IOdometry2DMsgs_reset_odometry_RPC_helper helper{};
    bool ok = yarp().write(helper, helper);
    return ok ? helper.reply.return_helper : yarp::dev::ReturnValue{};
}

// help method
std::vector<std::string> IOdometry2DMsgs::help(const std::string& functionName)
{
    bool showAll = (functionName == "--all");
    std::vector<std::string> helpString;
    if (showAll) {
        helpString.emplace_back("*** Available commands:");
        helpString.emplace_back(IOdometry2DMsgs_reset_odometry_RPC_helper::s_tag);
        helpString.emplace_back("help");
    } else {
        if (functionName == IOdometry2DMsgs_reset_odometry_RPC_helper::s_tag) {
            helpString.emplace_back(IOdometry2DMsgs_reset_odometry_RPC_helper::s_prototype);
        }
        if (functionName == "help") {
            helpString.emplace_back("std::vector<std::string> help(const std::string& functionName = \"--all\")");
            helpString.emplace_back("Return list of available commands, or help message for a specific function");
            helpString.emplace_back("@param functionName name of command for which to get a detailed description. If none or '--all' is provided, print list of available commands");
            helpString.emplace_back("@return list of strings (one string per line)");
        }
    }
    if (helpString.empty()) {
        helpString.emplace_back("Command not found");
    }
    return helpString;
}

// read from ConnectionReader
bool IOdometry2DMsgs::read(yarp::os::ConnectionReader& connection)
{
    constexpr size_t max_tag_len = 3;
    size_t tag_len = 1;

    yarp::os::idl::WireReader reader(connection);
    reader.expectAccept();
    if (!reader.readListHeader()) {
        reader.fail();
        return false;
    }

    std::string tag = reader.readTag(1);
    bool direct = (tag == "__direct__");
    if (direct) {
        tag = reader.readTag(1);
    }
    while (tag_len <= max_tag_len && !reader.isError()) {
        if (tag == IOdometry2DMsgs_reset_odometry_RPC_helper::s_tag) {
            IOdometry2DMsgs_reset_odometry_RPC_helper helper;
            if (!helper.cmd.readArgs(reader)) {
                return false;
            }

            helper.call(this);

            yarp::os::idl::WireWriter writer(reader);
            if (!helper.reply.write(writer)) {
                return false;
            }
            reader.accept();
            return true;
        }
        if (tag == "help") {
            std::string functionName;
            if (!reader.readString(functionName)) {
                functionName = "--all";
            }
            auto help_strings = help(functionName);
            yarp::os::idl::WireWriter writer(reader);
            if (!writer.isNull()) {
                if (!writer.writeListHeader(2)) {
                    return false;
                }
                if (!writer.writeTag("many", 1, 0)) {
                    return false;
                }
                if (!writer.writeListBegin(0, help_strings.size())) {
                    return false;
                }
                for (const auto& help_string : help_strings) {
                    if (!writer.writeString(help_string)) {
                        return false;
                    }
                }
                if (!writer.writeListEnd()) {
                    return false;
                }
            }
            reader.accept();
            return true;
        }
        if (reader.noMore()) {
            reader.fail();
            return false;
        }
        std::string next_tag = reader.readTag(1);
        if (next_tag.empty()) {
            break;
        }
        tag.append("_").append(next_tag);
        tag_len = std::count(tag.begin(), tag.end(), '_') + 1;
    }
    return false;
}
