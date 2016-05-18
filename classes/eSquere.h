/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   eSquere.h
 * Author: khalif
 *
 * Created on 23 Апрель 2016 г., 22:00
 */

#ifndef ESQUERE_H
#define ESQUERE_H
#include <string>
#include <ctime>
#include <iostream>
#include <sstream>

class eSquere {
public:
    
    eSquere();
    eSquere(std::string login);
    eSquere(std::string login, std::string password);
    eSquere(std::string login, std::string password, std::string hostname);
    eSquere(std::string login, std::string password, std::string hostname, std::string port);
    eSquere(std::string login, std::string password, std::string hostname, int port);
    eSquere(const eSquere& orig);
    virtual ~eSquere();
    
    eSquere *       SetHostName(std::string hostname);
    eSquere *       SetHostName();
    std::string     GetHostName();
    
    eSquere *       SetPort();
    eSquere *       SetPort(int port);
    eSquere *       SetPort(std::string port);
    int             GetPort();
    
    eSquere *       SetLogin();
    eSquere *       SetLogin(std::string login);
    
    eSquere *       SetPassword();
    eSquere *       SetPassword(std::string password);
    
    eSquere *       Connect();
    eSquere *       Login();
    eSquere *       Login(std::string login,std::string password);
    eSquere *       OpenChannel();
    eSquere *       CloseChannel();
    eSquere *       CloseConnection();
    eSquere *       DestroyConnection();
    eSquere *       DisConnect();
    eSquere *       ReConnect();
    eSquere *       ReConnectIfError();
    
    eSquere *       SetVHost(std::string hostname);
    eSquere *       SetVHost();
    std::string     GetVHost();
    
    eSquere *       SetPropertieFlags();
    eSquere *       SetPropertieFlags(amqp_flags_t flags);
    amqp_flags_t    GetPropertieFlags();
    
    eSquere *       SetContentType(std::string contenttype);
    eSquere *       SetContentType();
    std::string     GetContentType();
    
    eSquere *       SetMessageDeliveryModeOn();
    eSquere *       SetMessageDeliveryModeOff();
    bool            GetMessageDeliveryMode();
    
    eSquere *       SetExchange();
    eSquere *       SetExchange(std::string exch);
    std::string     GetExchange();
    eSquere *       SetDefaultExchangeDirect();
    eSquere *       SetRoutingKey();
    eSquere *       SetRoutingKey(std::string rout);
    std::string     GetRoutingKey();
    
    eSquere * SetMessage();
    eSquere * SetMessage(std::string message);
    std::string GetMessage();
    
    eSquere * SetQueueName();
    eSquere * SetQueueName(std::string message);
    eSquere * SetQueueName(amqp_bytes_t qname);
    std::string GetQueueName();
    
    eSquere * SendString();
    eSquere * SendString(std::string message);
    eSquere * SendString(std::string message,std::string routkey);
    eSquere * WriteString();
    eSquere * WriteString(std::string message);
    eSquere * WriteString(std::string message,std::string qname);
    
    std::string ToString(int param);
    std::string ToString(amqp_bytes_t param);
    
    eSquere *   SetPassive(bool mode);
    eSquere *   PassiveOn();
    eSquere *   PassiveOff();
    bool        GetPassive();
    
    eSquere *   SetDurable(bool mode);
    eSquere *   DurableOn();
    eSquere *   DurableOff();
    bool        GetDurable();
    
    eSquere *   SetExclusive(bool mode);
    eSquere *   ExclusiveOn();
    eSquere *   ExclusiveOff();
    bool        GetExclusive();
    
    eSquere *   SetAutoDelete(bool mode);
    eSquere *   AutoDeleteOn();
    eSquere *   AutoDeleteOff();
    bool        GetAutoDelete();
    
    eSquere *   CreateNewQueue();
    eSquere *   CreateNewQueue(std::string routekey);

    eSquere *   CreateQueue();
    eSquere *   CreateQueue(std::string routekey);

    eSquere * SetQueueParametrs();
    eSquere * SetQueueParametrs(std::string qparam);
    eSquere * SetQueueParametrs(amqp_table_t qparam);
    amqp_table_t GetQueueParametrs();

    eSquere * SetBindParametrs();
    eSquere * SetBindParametrs(std::string qparam);
    eSquere * SetBindParametrs(amqp_table_t qparam);
    amqp_table_t GetBindParametrs();

    eSquere * Bind();
    eSquere * Bind(std::string routekey);
    eSquere * Bind(std::string routekey, std::string qname);
    eSquere * Bind(std::string routekey, std::string qname, std::string exname);
    
    eSquere * Consume();
    eSquere * ConsumeCancel();
    eSquere * Consume(std::string qname);

    eSquere * ConsumeMessage();
    eSquere * ConsumeMessage(std::string qname);

    eSquere * ErrorOn();
    eSquere * ErrorOn(std::string errString);
    eSquere * ErrorOn(int errCode);
    eSquere * ErrorOn(int errCode,std::string errString);
    eSquere * ErrorOn(std::string errString,int errCode);
    eSquere * ErrorOff();
    eSquere * ErrorOff(std::string errString);
    eSquere * ErrorRPC();
    eSquere * ErrorRPC(std::string errString);
    eSquere * SetErrorMessage(std::string errString);
    std::string GetErrorMessage();
    eSquere * SetErrorCode(int errCode);
    int GetErrorCode();
    bool        IsError();
    eSquere * WriteErrorString();
    eSquere * PrintString(std::string Str);
    
    eSquere * SetTimeOut();
    eSquere * SetTimeOut(int sec);
    eSquere * SetTimeOut(int sec, int msec);
    struct timeval GetTimeOut();
    struct timeval * GetTimeOutPointer();
private:
    bool ErrorStatus;
    std::string 
          hostname,
          login,
          password,
          vhost,
          exchange,
          queuename,
          routingkey, 
          messagebody,
          ErrorMessage;
  int port, status, ErrorCode ;
  bool DeliveryMode,
        passive,
        durable,
        exclusive,
        auto_delete;
  amqp_rpc_reply_t statusAMPQ;
  amqp_socket_t *socket ;
  amqp_connection_state_t connect;
  amqp_basic_properties_t Proprties;
  std::string ContentType;
  amqp_table_t QueueParametrs;
  amqp_table_t BindParametrs;
  amqp_queue_declare_ok_t *DeclareQueueResult;
  amqp_envelope_t Message;
  amqp_envelope_t* MessagePointer;
  struct timeval timeout;
  
  eSquere* InitNull();
  eSquere* SetStatusErrorAMPQ(std::string ErrorString);
  eSquere* NewConnection();
  eSquere* NewSocket();
  eSquere* NewSocket(amqp_connection_state_t connect);
  eSquere* GetRpcReply();

};

#endif /* ESQUERE_H */

