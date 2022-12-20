#include <iostream>
#include <stdlib.h>
#include <string>
#include <ctype.h>
#include <stdexcept>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <QCommandLineParser>
#include <QDebug>

#include "tcp_server.h"
//#include "tcp_client.h"

typedef unsigned short int uint2_t;

#define LISTEN_QUEUE 3

enum command_line_parsing_result
{
    COMMAND_LINE_OK = 0,
    COMMAND_LINE_ERROR,
    COMMAND_LINE_HELP_REQUESTED
};

struct arguments
{
    QString type;
    QString server_ip_addr;
    uint2_t server_port;
};

bool isvalid_port_number(QString&);
bool isvalid_ip_addr(QString&);

command_line_parsing_result command_line_parser(QCommandLineParser&, struct arguments*, QString*);

int main(int argc, char *argv[])
{
    QCommandLineParser cmd_parser;
    QCoreApplication app(argc, argv);
    QString error_message;
    
    struct arguments* args;
    struct sockaddr_in* srv_addr = new struct sockaddr_in;

    try
    {
        args = new struct arguments;
    }
    catch(const std::bad_alloc& ex)
    {
        std::cerr << "couldn't allocate memory for the argument struct\n";
    }
    
    uint2_t result;
    try
    {
        result = command_line_parser(cmd_parser, args, &error_message);
    }
    catch(const std::invalid_argument& ex)
    {
        std::cerr << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    
    switch(result)
    {
    case COMMAND_LINE_OK:
        break;
    case COMMAND_LINE_HELP_REQUESTED:
        cmd_parser.showHelp(0);
        break;
    case COMMAND_LINE_ERROR:
        std::cerr << error_message.toStdString() << std::endl;
        break;
    default:
        std::cerr << "Something went wrong\n";
        exit(EXIT_FAILURE);
    }

    // conversion:                        <-----QString----->  <--string--> <-char*->
    srv_addr->sin_addr.s_addr = inet_addr(args->server_ip_addr.toStdString().c_str());
    srv_addr->sin_port = htons(args->server_port);
    srv_addr->sin_family = AF_INET;

    std::string _type = args->type.toStdString();
    if (_type == "server")
    {
        tcp_server *srv = new tcp_server((struct sockaddr*)srv_addr, LISTEN_QUEUE, 1024, 1024);
        srv->start();
    }
    else if (_type == "client")
    {
        ;
    }
    
    std::cout << "After server\n";

    return 0;
}

bool isvalid_port_number(QString& input_port)
{
    std::string input = input_port.toStdString();
    
    for (int i = 0; i < input.size(); ++i)
    {
         if (!isdigit(input[i]))
             return false;
    }

    long long port_number = input_port.toLongLong();
    
    if (port_number < 0 || port_number > 65535)
        return false;

    return true;
}

bool isvalid_ip_addr(QString& input_addr)
{
    std::string input = input_addr.toStdString();
    
    for (int i = 0; i < input.size(); ++i)
    {
        if (!isdigit(input[i]) && input[i] != '.')
            return false;
    }
    
    return true;
}

command_line_parsing_result command_line_parser(QCommandLineParser& parser, struct arguments* args, QString* error_message)
{
    // getting type of the using from the user (the program is server or client)
    const QCommandLineOption type_opt("t", "type of the program (the type can be \"server\" or \"client\")", "type");
    parser.addOption(type_opt);
    
    // server ip address option
    const QCommandLineOption srv_addr_opt("s", "server ip address", "SRV_ADDR", "127.0.0.1");
    parser.addOption(srv_addr_opt);
    
    // server port option
    const QCommandLineOption srv_port_opt("p", "server port", "SRV_PORT", "8008");
    parser.addOption(srv_port_opt);

    // getting help
    const QCommandLineOption help = parser.addHelpOption();
    
    if (!parser.parse(QCoreApplication::arguments()))
    {
        *error_message = parser.errorText();
        return COMMAND_LINE_ERROR;
    }

    if (parser.isSet(help))
        return COMMAND_LINE_HELP_REQUESTED;
    
    if (parser.isSet(type_opt))
    {
        args->type = parser.value(type_opt);
        args->type = args->type.toLower();
    }
    
    if (parser.isSet(srv_addr_opt))
    {
        args->server_ip_addr = parser.value(srv_addr_opt);
        
        if (!isvalid_ip_addr(args->server_ip_addr))
            throw std::invalid_argument("input ip address must be in XXX.XXX.XXX.XXX format");
    }

    if (parser.isSet(srv_port_opt))
    {
        QString port_number = parser.value(srv_port_opt);
        
        if (isvalid_port_number(port_number))
            args->server_port = port_number.toInt();
        else
            throw std::invalid_argument("the port number must an integer from 0 to 65635");
    }

    return COMMAND_LINE_OK;
}