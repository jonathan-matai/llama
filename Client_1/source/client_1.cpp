﻿#include <llama.h>

#include "calculator.h"

#include <typeindex>
#include <cstdio>

int main()
{
    /*
    printf("Enter IP Version: ");
    int i;
    scanf_s("%d", &i);

    llama::ClientSocket socket = llama::createClientSocket();

    printf("Enter hostname of server: ");
    char buffer[128];
    scanf_s("%s", buffer, 128);

    socket->connectServer(buffer, "2020", i == 4 ? true : false);
    */

    /*
    llama::EventBus bus = llama::createEventBus();

    llama::logfile()->print(llama::Colors::WHITE, "Created event bus!");

    std::shared_ptr<CalculatorFilter> filter = std::make_shared<CalculatorFilter>(bus);

    std::shared_ptr<Calculator> calc = std::make_shared<Calculator>(filter);
    std::shared_ptr<Console> con = std::make_shared<Console>(bus);

    calc->addToDefaultBus();
    con->addToDefaultBus();

    con->run();
    */
    
    
    llama::EventBus calculatorBus = llama::createEventBus();
    llama::EventBus printerbus = llama::createEventBus();

    llama::logfile()->print(llama::Colors::WHITE, "Created event busses!");

    std::shared_ptr<CalculatorBusFilter> filter = std::make_shared<CalculatorBusFilter>(calculatorBus, printerbus);

    std::shared_ptr<Calculator> calc = std::make_shared<Calculator>(calculatorBus);
    std::shared_ptr<Console> con = std::make_shared<Console>(printerbus);

    calc->addToDefaultBus();
    con->addToDefaultBus();

    con->run();
    /*
    llama::Logfile log = llama::createLogfile(u8"Client_1", u8"log.html");

    log->print(llama::Colors::YELLOW, u8"Übergrößenträger");
    log->print(llama::Colors::MAGENTA, LLAMA_DEBUG_INFO, "The %s goes skrra", "ting");

    llama::Table table("Playlist", llama::Colors::WHITE, { "Title", "Artist", "Genre" });
    table.addRow(llama::Colors::GREEN, { "Owner of a Lonely Heart", "Yes", "Rock" });
    table.addRow({ { llama::Colors::RED, "Don't beg" }, { llama::Colors::WHITE, "Synx, CruciA" }, { llama::Colors::BLUE, "Dubstep" } });
    table.addRow(llama::Colors::WHITE, { "Halo", "Martin O'Donnell, Michael Salvatori", u8"Filmmusik" });

    log->print(table);
    log->print(table, true);
    */
    return 0;
}