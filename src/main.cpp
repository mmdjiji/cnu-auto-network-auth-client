// main.cpp
// This file is a part of cnu-auto-network-auth-client project.
// Author: mmdjiji (JiJi)
// GitHub: https://github.com/mmdjiji/cnu-auto-network-auth-client
// License: GPL v3.0
// Author's Blog: https://mmdjiji.com

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <regex>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "cJSON.h"

#define BUFFER_SIZE (4096)

#define CONFIG_PATH ("config.json")

std::string curl(const char *ip_addr, int port, std::string msg) {
  WSADATA wsaData;
  SOCKET client;
  SOCKADDR_IN remoteServer;
  char buffer[BUFFER_SIZE];
  std::string ret;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
  client = socket(AF_INET, SOCK_STREAM, 0);
  remoteServer.sin_addr.S_un.S_addr = inet_addr(ip_addr);
  remoteServer.sin_family = AF_INET; // IPv4
  remoteServer.sin_port = htons(port);
  connect(client, (SOCKADDR*)&remoteServer, sizeof(SOCKADDR));
  send(client, msg.c_str(), msg.length(), 0);
  recv(client, buffer, BUFFER_SIZE, 0);
  ret += std::string(buffer);
  recv(client, buffer, BUFFER_SIZE, 0);
  ret += std::string(buffer);
  closesocket(client);
  WSACleanup();
  return ret;
}

std::string getPostData(std::string hostname, std::string username, std::string password) {
  std::string body = "DDDDD=" + username + "&upass=" + password + "&0MKKey=";
  std::string ret = "";
  ret += "POST / HTTP/1.1\r\n";
  ret += "host: " + hostname + "\r\n";
  ret += "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.114 Safari/537.36 Edg/89.0.774.76\r\n";
  ret += "Accept: */*\r\n";
  ret += "Content-Length: " + std::to_string(body.length()) + "\r\n";
  ret += "Content-Type: application/x-www-form-urlencoded\r\n\r\n";
  ret += body;
  return ret;
}

bool login(std::string hostname, std::string username, std::string password) {
  std::string req, recv;
  std::regex v46ip("v46ip='.+'");
  std::regex msga("msga='.+'");
  std::smatch match;
  req = getPostData(hostname, username, password);
  recv = curl(hostname.c_str(), 80, req);
  if(std::regex_search(recv, match, v46ip)) {
    std::cout << "Login success! " << match.str(0) << "." << std::endl;
    return true;
  } else {
    if(std::regex_search(recv, match, msga)) {
      std::cout << "Login failed, " << match.str(0) << "." << std::endl;
    }
  }
  return false;
}

const char *read_json() {
  FILE *fptr = fopen(CONFIG_PATH, "r");
  int file_size;
  if(fptr) {
    fseek(fptr, 0L, SEEK_END);
    file_size = ftell(fptr);
    char *retval = (char *)malloc(file_size * sizeof(char));
    memset(retval, 0, file_size * sizeof(char));
    fseek(fptr, 0, SEEK_SET);
    fread(retval, file_size, 1, fptr);
    fclose(fptr);
    return retval;
  }
  return NULL;
}

const char *read_field_string(cJSON *src, const char *string) {
  cJSON *result = cJSON_GetObjectItem(src, string);
  return result->valuestring;
}

int read_field_int(cJSON *src, const char *string) {
  cJSON *result = cJSON_GetObjectItem(src, string);
  return result->valueint;
}

bool write_json(char * src) {
  FILE *config_file = fopen(CONFIG_PATH, "w");
  if(config_file) {
    fprintf(config_file, "%s", src);
    fclose(config_file);
    return true;
  }
  return false;
}


int main() {
  const char *config_src = read_json();
  cJSON *config = cJSON_Parse(config_src);
  std::string username, password;
  
  if(config) {
    username = std::string(read_field_string(config, "username"));
    password = std::string(read_field_string(config, "password"));
  } else {
    std::cout << "Input your username: ";
    std::cin >> username;
    std::cout << "Input your password: ";
    std::cin >> password;

    config = cJSON_CreateObject();
    cJSON *config_username = cJSON_CreateString(username.c_str());
    cJSON *config_password = cJSON_CreateString(password.c_str());

    cJSON_AddItemToObject(config, "username", config_username);
    cJSON_AddItemToObject(config, "password", config_password);
    
    if(write_json(cJSON_Print(config))) {
      std::cout << "Write config success." << std::endl;
    } else {
      std::cout << "Write config failed." << std::endl;
      return 0;
    }
  }

  if(!login("10.10.10.9", username, password)) {
    if(!login("192.168.1.91", username, password)) {
      std::cout << "Cannot connect the network automatically." << std::endl;
    }
  }
  
  return 0;
}
