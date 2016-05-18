/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   eSquere.cpp
 * Author: khalif
 * 
 * Created on 23 Апрель 2016 г., 22:00
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>

#include <amqp.h>
#include <amqp_framing.h>
#include <amqp_tcp_socket.h>

#include "eSquere.h"
eSquere::eSquere() {
    this->InitNull();
}

eSquere::eSquere(const eSquere& orig) {
    this->InitNull();
    this->SetHostName(orig.hostname)
            ->SetPort(orig.port)
            ->SetLogin(orig.login)
            ->SetPassword(orig.password)
            ->SetVHost(orig.vhost);
}

eSquere::eSquere(std::string login){
    this->InitNull()
            ->SetLogin(login);
}

eSquere::eSquere(std::string login, std::string password){
    this->InitNull()
            ->SetLogin(login)
            ->SetPassword(password);
}

eSquere::eSquere(std::string login, std::string password, std::string hostname){
    this->InitNull()
            ->SetLogin(login)
            ->SetPassword(password)
            ->SetHostName(hostname);
}

eSquere::eSquere(std::string login, std::string password, std::string hostname, std::string port){
    this->InitNull()
            ->SetLogin(login)
            ->SetPassword(password)
            ->SetHostName(hostname)
            ->SetPort(port);
}

eSquere::eSquere(std::string login, std::string password, std::string hostname, int port){
    this->InitNull()
            ->SetLogin(login)
            ->SetPassword(password)
            ->SetHostName(hostname)
            ->SetPort(port);
}

eSquere::~eSquere() {
    this->DisConnect();
}

eSquere* eSquere::InitNull(){
    this->socket=NULL;
    this->ErrorStatus=false;
    this->statusAMPQ.library_error=0;
    this->statusAMPQ.reply_type=AMQP_RESPONSE_NONE;
    this->MessagePointer=&this->Message;
    return this->SetHostName()
            ->SetPort()
            ->SetLogin()
            ->SetPassword()
            ->SetVHost()
            ->SetContentType()
            ->SetPropertieFlags()
            ->SetMessageDeliveryModeOff()
            ->SetExchange()
            ->SetRoutingKey()
            ->SetMessage()
            ->PassiveOff()
            ->DurableOff()
            ->AutoDeleteOn()
            ->ExclusiveOff()
            ->SetQueueParametrs()
            ->SetBindParametrs()
            ->SetTimeOut();
}

eSquere * eSquere::SetHostName(std::string hostname){
    if(hostname==""){
        this->hostname="localhost";
    }else{
        this->hostname=hostname;
    }
    return this;
}
eSquere * eSquere::SetHostName(){
    this->hostname="localhost";
    return this;
}
 std::string eSquere::GetHostName(){
    return this->hostname;
}
 
eSquere * eSquere::SetVHost(std::string vhost){
    if(hostname==""){
        this->vhost  ="/";
    }else{
        this->hostname=vhost;
    }
    return this;
}
eSquere * eSquere::SetVHost(){
    this->vhost="/";
    return this;
}
 std::string eSquere::GetVHost(){
    return this->vhost;
}
 

 
eSquere * eSquere::SetPort(){
    this->port=5672;
    return this;
}
eSquere * eSquere::SetPort(int port){
    if(port==0){
        this->port=5672;
    }
    else{
        this->port=port;
    }
    return this;
}
eSquere * eSquere::SetPort(std::string port){
//    this->port=atoi(port.c_str());
    this->SetPort(atoi(port.c_str()));
    return this;
}
int  eSquere::GetPort(){
    return this->port;
}


eSquere * eSquere::SetLogin(std::string login){
    this->login=login;
    return this;
}
eSquere * eSquere::SetLogin(){
    this->login="guest";
    return this;
}
eSquere * eSquere::SetPassword(std::string password){
    this->password=password;
    return this;
}
eSquere * eSquere::SetPassword(){
    this->password="guest";
    return this;
}

eSquere* eSquere::NewConnection(){
    this->connect=amqp_new_connection();
    return this;
}

eSquere* eSquere::NewSocket(){
    return this->NewSocket(this->connect);
}

eSquere* eSquere::NewSocket(amqp_connection_state_t conn){
    this->socket = amqp_tcp_socket_new(conn);
    if(!this->socket) return this->ErrorOn("Error creating TCP socket");
    if(amqp_socket_open(this->socket, this->hostname.c_str(), this->port)) return this->ErrorOn("Error opening TCP socket");
    return this;
}

bool     eSquere::IsError(){
    return this->ErrorStatus;
}
eSquere *   eSquere::Login(){
    if(this->IsError()) return this;
    this->statusAMPQ=amqp_login(this->connect, this->vhost.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, this->login.c_str(), this->password.c_str());
    return this->SetStatusErrorAMPQ("Login vhost:"+this->vhost+"; login:"+this->login);
}
eSquere *   eSquere::Login(std::string login,std::string password){
    return this->SetLogin(login)->SetPassword(password)->Login();
}
eSquere *   eSquere::OpenChannel(){
    if(this->IsError()) return this;
    amqp_channel_open(this->connect, 1);
    this->GetRpcReply()->SetStatusErrorAMPQ("Open channel");
    return this;
}

eSquere* eSquere::Connect(){
    return this->NewConnection()->NewSocket()->Login()->OpenChannel();
}

eSquere* eSquere::ReConnect(){
    return this->ErrorOff()->DisConnect()->Connect();
}

eSquere* eSquere::ReConnectIfError(){
    if(this->IsError()) this->ErrorOff()->DisConnect()->Connect();
    return this;
}

eSquere* eSquere::GetRpcReply(){
    this->statusAMPQ=amqp_get_rpc_reply(this->connect);
    return this;
}

eSquere *   eSquere::CloseChannel(){
    this->statusAMPQ=amqp_channel_close(this->connect, 1, AMQP_REPLY_SUCCESS);
    return this->SetStatusErrorAMPQ("Close channel");
}

eSquere *   eSquere::CloseConnection(){
    this->statusAMPQ=amqp_connection_close(this->connect, AMQP_REPLY_SUCCESS);
    return this->SetStatusErrorAMPQ("Close connection") ;
}

eSquere *   eSquere::DestroyConnection(){
    int x=amqp_destroy_connection(this->connect);
    if(x<0){
        this->ErrorStatus=true;
        this->ErrorMessage=amqp_error_string2(x);
    }
    return this;
}

eSquere* eSquere::DisConnect(){
    if(this->CloseChannel()->IsError()) return this;
    if(this->CloseConnection()->IsError()) return this;
    return this->DestroyConnection();
}

eSquere* eSquere::SetStatusErrorAMPQ(std::string ErrorString){
    /**< the library got an EOF from the socket */
    if(this->statusAMPQ.reply_type==AMQP_RESPONSE_NONE) return this->ErrorOn(ErrorString+" the library got an EOF from the socket",AMQP_RESPONSE_NONE);
    /**< response normal, the RPC completed successfully */
    if(this->statusAMPQ.reply_type==AMQP_RESPONSE_NORMAL) return this->ErrorOff(ErrorString+" response normal, the RPC completed successfully");
    /**< library error, an error occurred in the library, examine the library_error */
    if(this->statusAMPQ.reply_type==AMQP_RESPONSE_LIBRARY_EXCEPTION) return this->ErrorOn(ErrorString+"  library error, an error occurred in the library, examine the library_error "+amqp_error_string2(this->statusAMPQ.library_error),AMQP_RESPONSE_LIBRARY_EXCEPTION);
    /**< server exception, the broker returned an error, check replay */
    if(this->statusAMPQ.reply_type==AMQP_RESPONSE_SERVER_EXCEPTION) return this->ErrorOn(ErrorString+"  server exception, the broker returned an error, check replay",AMQP_RESPONSE_SERVER_EXCEPTION);
    return this->ErrorOn(ErrorString+" unknown Error",this->statusAMPQ.reply_type);
}
eSquere * eSquere::SetPropertieFlags(){
    this->Proprties._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    return this;
}

eSquere * eSquere::SetPropertieFlags(amqp_flags_t flags){
    this->Proprties._flags = flags;
    return this;
}

amqp_flags_t eSquere::GetPropertieFlags(){
    return this->Proprties._flags;
}

eSquere * eSquere::SetContentType(){
    this->ContentType="text/plain";
    this->Proprties.content_type = amqp_cstring_bytes("text/plain");
    return this;
}
eSquere * eSquere::SetContentType(std::string contenttype){
    this->ContentType=contenttype;
    this->Proprties.content_type = amqp_cstring_bytes(contenttype.c_str());
    return this;
}

std::string eSquere::GetContentType(){
    return this->ContentType;
}

eSquere * eSquere::SetMessageDeliveryModeOn(){
    this->DeliveryMode=true;
    this->Proprties.delivery_mode = 2;
    return this;
}

eSquere * eSquere::SetMessageDeliveryModeOff(){
    this->DeliveryMode=false;
    this->Proprties.delivery_mode = 1;
    return this;
}

bool  eSquere::GetMessageDeliveryMode(){
    return this->DeliveryMode;
}

eSquere * eSquere::SetExchange(){
    this->exchange="";
    return this;
}
eSquere * eSquere::SetDefaultExchangeDirect(){
    this->exchange="amq.direct";
    return this;
}
eSquere * eSquere::SetExchange(std::string exch){
    this->exchange=exch;
    return this;
}
std::string eSquere::GetExchange(){
    return this->exchange;
}
    
eSquere * eSquere::SetRoutingKey(){
    return this->SetRoutingKey("");
}
eSquere * eSquere::SetRoutingKey(std::string rout){
    if(rout==""){
        if(this->GetQueueName()==""){
            this->routingkey   ="aaa_test";
        }else{
            this->routingkey=this->GetQueueName();
        }
    }else{
        this->routingkey   =rout;
    }
    return this;
}
std::string eSquere::GetRoutingKey(){
    return this->routingkey;
}

eSquere * eSquere::SetMessage(){
    this->messagebody   ="null";
    return this;
}
eSquere * eSquere::SetMessage(std::string message){
    this->messagebody   =message;
    return this;
}
std::string eSquere::GetMessage(){
    return this->messagebody;
}

eSquere * eSquere::SendString(){
    if(this->IsError()) return this;
    int code=amqp_basic_publish(
            this->connect,
            1,
            amqp_cstring_bytes(this->exchange.c_str()),
            amqp_cstring_bytes(this->routingkey.c_str()),
            0,
            0,
            &this->Proprties,
            amqp_cstring_bytes(this->messagebody.c_str()));
    if(code<0) return this->ErrorOn(code,amqp_error_string2(code));
    return this->ErrorOff("Send message");
}
eSquere * eSquere::SendString(std::string message){
    if(this->IsError()) return this;
    int code=amqp_basic_publish(
            this->connect,
            1,amqp_cstring_bytes(this->exchange.c_str()),
            amqp_cstring_bytes(this->routingkey.c_str()),
            0,
            0,
            &this->Proprties,
            amqp_cstring_bytes(message.c_str()));
    if(code<0) return this->ErrorOn(code,amqp_error_string2(code));
    return this->ErrorOff("Send message");
}

eSquere * eSquere::SendString(std::string message,std::string routkey){
    if(this->IsError()) return this;
    int code=amqp_basic_publish(
            this->connect,
            1,amqp_cstring_bytes(this->exchange.c_str()),
            amqp_cstring_bytes(routkey.c_str()),
            0,
            0,
            &this->Proprties,
            amqp_cstring_bytes(message.c_str()));
    if(code<0) return this->ErrorOn(code,amqp_error_string2(code));
    return this->ErrorOff("Send message");
}

eSquere * eSquere::WriteString(){
    return this->WriteString(this->GetMessage(),this->GetQueueName());
}

eSquere * eSquere::WriteString(std::string message){
    return this->WriteString(message,this->GetQueueName());
}

eSquere * eSquere::WriteString(std::string message,std::string qname){
    if(this->IsError()) return this;
    if(qname==""){
        return this->ErrorOn("Not set Queue Name");
    }
    int code=amqp_basic_publish(
            this->connect,
            1,
            amqp_cstring_bytes(""),
            amqp_cstring_bytes(qname.c_str()),
            0,
            0,
            &this->Proprties,
            amqp_cstring_bytes(message.c_str()));
    if(code<0) return this->ErrorOn(code,amqp_error_string2(code));
    return this->ErrorOff("Write message");
}
eSquere * eSquere::WriteErrorString(){
  std::cout << this->GetErrorMessage() << std::endl;
  return this;
}

std::string eSquere::ToString(int param){
    std::stringstream ss;
    ss << param;
    return ss.str();
}

std::string eSquere::ToString(amqp_bytes_t param){
    std::stringstream ss;
    char * str=(char*)param.bytes;
    for(int i=0;i<param.len;i++){
        ss << str[i];
    }
//    ss << param;
    return ss.str();
}

eSquere * eSquere::SetQueueName(){
    this->queuename="";
    return this;
}

eSquere * eSquere::SetQueueName(std::string qname){
    this->queuename=qname;
    return this;
}
// amqp_bytes_t q
eSquere * eSquere::SetQueueName(amqp_bytes_t qname){
    this->queuename=this->ToString(qname);
    return this;
}

std::string eSquere::GetQueueName(){
    return this->queuename;
}

eSquere *   eSquere::SetPassive(bool mode){
    if(mode) return this->PassiveOn();
    return this->PassiveOff();
}
eSquere *   eSquere::PassiveOn(){
    this->passive=true;
    return this;
}
eSquere *   eSquere::PassiveOff(){
    this->passive=false;
    return this;
}
bool        eSquere::GetPassive(){
    return this->passive;
}

eSquere *   eSquere::SetDurable(bool mode){
    if(mode) return this->DurableOn();
    return this->DurableOff();
}
eSquere *   eSquere::DurableOn(){
    this->durable=true;
    return this;
}
eSquere *   eSquere::DurableOff(){
    this->durable=false;
    return this;
}
bool        eSquere::GetDurable(){
    return this->durable;
}

eSquere *   eSquere::SetExclusive(bool mode){
    if(mode) return this->ExclusiveOn();
    return this->ExclusiveOff();
}
eSquere *   eSquere::ExclusiveOn(){
    this->exclusive=true;
    return this;
}
eSquere *   eSquere::ExclusiveOff(){
    this->exclusive=false;
    return this;
}
bool        eSquere::GetExclusive(){
    return this->exclusive;
}

eSquere *   eSquere::SetAutoDelete(bool mode){
    if(mode) return this->AutoDeleteOn();
    return this->AutoDeleteOff();
}
eSquere *   eSquere::AutoDeleteOn(){
    this->auto_delete=true;
    return this;
}
eSquere *   eSquere::AutoDeleteOff(){
    this->auto_delete=false;
    return this;
}
bool        eSquere::GetAutoDelete(){
    return this->auto_delete;
}
/*
AMQP_CALL amqp_queue_declare(
        amqp_connection_state_t state, 
        amqp_channel_t channel, 
        amqp_bytes_t queue, 
        amqp_boolean_t passive, 
        amqp_boolean_t durable, 
        amqp_boolean_t exclusive, 
        amqp_boolean_t auto_delete, 
        amqp_table_t arguments
        )
*/
eSquere *   eSquere::CreateNewQueue(){
    if(this->IsError()) return this;
    this->DeclareQueueResult = amqp_queue_declare(
            this->connect,
            1,
            amqp_empty_bytes,
            this->GetPassive(),
            this->GetDurable(),
            this->GetExclusive(),
            this->GetAutoDelete(),
            this->GetQueueParametrs());
    if(this->GetRpcReply()->SetStatusErrorAMPQ("Declaring queue")->IsError()) return this;
    amqp_bytes_t q = amqp_bytes_malloc_dup(this->DeclareQueueResult->queue);
    if(q.bytes==NULL) return this->ErrorOn("Out of memory while copying queue name");
    this->SetQueueName(q)->SetRoutingKey()->ErrorOff("Set Queue Name :"+this->GetQueueName());
    return this;
}
eSquere *   eSquere::CreateNewQueue(std::string routekey){
    return this->CreateNewQueue()->SetRoutingKey(routekey);
}

eSquere *   eSquere::CreateQueue(){
    return this->CreateQueue(this->GetQueueName());
}

eSquere *   eSquere::CreateQueue(std::string qname){
    if(this->IsError()) return this;
    this->DeclareQueueResult = amqp_queue_declare(
            this->connect,
            1,
            amqp_cstring_bytes(qname.c_str()),
            this->GetPassive(),
            this->GetDurable(),
            this->GetExclusive(),
            this->GetAutoDelete(),
            this->GetQueueParametrs());
    if(this->GetRpcReply()->SetStatusErrorAMPQ("Declaring queue "+qname)->IsError()) return this;
    amqp_bytes_t q = amqp_bytes_malloc_dup(this->DeclareQueueResult->queue);
    if(q.bytes==NULL) return this->ErrorOn("Out of memory while copying queue name "+qname);
    return this->SetQueueName(qname)->ErrorOff("Create Queue :"+qname);
}

eSquere * eSquere::SetQueueParametrs(){
    this->QueueParametrs=amqp_empty_table;
    return this;
}

eSquere * eSquere::SetQueueParametrs(std::string qparam){
    return this;
}

eSquere * eSquere::SetQueueParametrs(amqp_table_t qparam){
    this->QueueParametrs=qparam;
    return this;
}

amqp_table_t eSquere::GetQueueParametrs(){
    return this->QueueParametrs;
}

eSquere * eSquere::SetBindParametrs(){
    this->BindParametrs=amqp_empty_table;
    return this;
}

eSquere * eSquere::SetBindParametrs(std::string qparam){
    this->BindParametrs=amqp_empty_table;
    return this;
}

eSquere * eSquere::SetBindParametrs(amqp_table_t qparam){
    this->BindParametrs=qparam;
    return this;
}

amqp_table_t eSquere::GetBindParametrs(){
    return this->BindParametrs;
}

eSquere * eSquere::Bind(){
    if(this->IsError()) return this;
    amqp_queue_bind(
          this->connect,
          1,
          amqp_cstring_bytes(this->queuename.c_str()),
          amqp_cstring_bytes(this->exchange.c_str()),
          amqp_cstring_bytes(this->routingkey.c_str()),
          this->BindParametrs);
  return this->ErrorRPC("Binding queue");
}

eSquere * eSquere::Bind(std::string routekey){
    if(this->IsError()) return this;
    amqp_queue_bind(
          this->connect,
          1,
          amqp_cstring_bytes(this->queuename.c_str()),
          amqp_cstring_bytes(this->exchange.c_str()),
          amqp_cstring_bytes(routekey.c_str()),
          this->BindParametrs);
  return this->ErrorRPC("Binding queue");
}
eSquere * eSquere::Bind(std::string routekey, std::string qname){
    if(this->IsError()) return this;
    amqp_queue_bind(
          this->connect,
          1,
          amqp_cstring_bytes(qname.c_str()),
          amqp_cstring_bytes(this->exchange.c_str()),
          amqp_cstring_bytes(routekey.c_str()),
          this->BindParametrs);
  return this->ErrorRPC("Binding queue");
}
eSquere * eSquere::Bind(std::string routekey, std::string qname, std::string exname){
    if(this->IsError()) return this;
    amqp_queue_bind(
          this->connect,
          1,
          amqp_cstring_bytes(qname.c_str()),
          amqp_cstring_bytes(exname.c_str()),
          amqp_cstring_bytes(routekey.c_str()),
          this->BindParametrs);
  return this->ErrorRPC("Binding queue");
}

eSquere * eSquere::ErrorOn(){
    this->ErrorStatus=true;
    return this;
}
eSquere * eSquere::ErrorOn(std::string errString){
    return this->ErrorOn()->SetErrorMessage(errString);
}
eSquere * eSquere::ErrorOn(int errCode){
    return this->ErrorOn()->SetErrorCode(errCode);
}
eSquere * eSquere::ErrorOn(int errCode,std::string errString){
    return this->ErrorOn()->SetErrorCode(errCode)->SetErrorMessage(errString);
}
eSquere * eSquere::ErrorOn(std::string errString,int errCode){
    return this->ErrorOn()->SetErrorCode(errCode)->SetErrorMessage(errString);
}
eSquere * eSquere::ErrorOff(){
    this->ErrorCode=0;
    this->ErrorStatus=false;
    return this;
}
eSquere * eSquere::ErrorOff(std::string errString){
    return this->ErrorOff()->SetErrorMessage(errString);
}

eSquere * eSquere::SetErrorMessage(std::string errString){
    this->ErrorMessage=errString;
    return this;
}

std::string eSquere::GetErrorMessage(){
    return this->ErrorMessage;
}
eSquere * eSquere::SetErrorCode(int errCode){
    this->ErrorCode=errCode;
    return this;
}
int eSquere::GetErrorCode(){
    return this->ErrorCode;
}
eSquere * eSquere::ErrorRPC(){
    return this->GetRpcReply()->SetStatusErrorAMPQ("");
}
eSquere * eSquere::ErrorRPC(std::string errString){
    return this->GetRpcReply()->SetStatusErrorAMPQ(errString);
}
eSquere * eSquere::Consume(){
    return this->Consume(this->GetQueueName());
}

eSquere * eSquere::Consume(std::string qname){
    if(this->IsError()) return this;
// amqp_basic_consume(
//    amqp_connection_state_t state, 
//    amqp_channel_t channel, 
//    amqp_bytes_t queue, 
//    amqp_bytes_t consumer_tag, 
//    amqp_boolean_t no_local, 
//    amqp_boolean_t no_ack, 
//    amqp_boolean_t exclusive, 
//    amqp_table_t arguments);    
    amqp_basic_consume(
            this->connect, 
            1, 
            amqp_cstring_bytes(qname.c_str()), 
            amqp_empty_bytes, 
            0, 
            0, 
            0, 
            amqp_empty_table);
    return this->ErrorRPC("Consuming "+qname);
    
}

eSquere * eSquere::ConsumeCancel(){
    if(this->IsError()) return this;
//    amqp_basic_cancel(amqp_connection_state_t state, amqp_channel_t channel, amqp_bytes_t consumer_tag);
    amqp_basic_cancel(
            this->connect, 
            1, 
            amqp_empty_bytes);
    return this->ErrorRPC("Consuming Cancel");
}

eSquere * eSquere::ConsumeMessage(){
    return this->ConsumeMessage(this->GetQueueName());
}
eSquere * eSquere::ConsumeMessage(std::string qname){
    if(this->IsError()) return this;
    amqp_maybe_release_buffers(this->connect);
    this->statusAMPQ=amqp_consume_message(this->connect, &this->Message, this->GetTimeOutPointer(), 0);
    this->SetStatusErrorAMPQ("Read Message "+qname);
    if(this->IsError()) return this;
    this->SetMessage(this->ToString(this->Message.message.body));
    return this;
}

eSquere * eSquere::PrintString(std::string Str){
    std::cout << Str << std::endl;
    return this;
}

eSquere * eSquere::SetTimeOut(){
    return this->SetTimeOut(0,600);
}
eSquere * eSquere::SetTimeOut(int sec){
    return this->SetTimeOut(sec,0);
}
eSquere * eSquere::SetTimeOut(int sec, int msec){
    this->timeout.tv_sec=sec;
    this->timeout.tv_usec=msec*1000;
    return this;
}
struct timeval eSquere::GetTimeOut(){
    return this->timeout;
}
struct timeval * eSquere::GetTimeOutPointer(){
    return &this->timeout;
}
// amqp_basic_get