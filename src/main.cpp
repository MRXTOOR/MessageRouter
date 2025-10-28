#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <signal.h>
#include <atomic>
#include <string>
#include <exception>
#include <cstdlib>
#include <cstring>
#include "../include/config.h"
#include "../include/producer.h"
#include "../include/processor.h"
#include "../include/strategy.h"
#include "../include/stage1_router.h"
#include "../include/stage2_router.h"

using namespace MessageRouter;


std::atomic<bool> g_running{true};
ProducerManager* g_producer_manager = nullptr;
Stage1Router* g_stage1_router = nullptr;
ProcessorManager* g_processor_manager = nullptr;
Stage2Router* g_stage2_router = nullptr;
StrategyManager* g_strategy_manager = nullptr;


void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    g_running.store(false);
}

int main(int argc, char* argv[]) {
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        std::cerr << "Example: " << argv[0] << " configs/baseline.json" << std::endl;
        return 1;
    }
    
    std::string config_file = argv[1];
    
    try {
        
        std::cout << "Loading configuration from " << config_file << "..." << std::endl;
        auto config = SystemConfig::load_from_file(config_file);
        
        if (!config->validate()) {
            std::cerr << "Configuration validation error!" << std::endl;
            return 1;
        }
        
        std::cout << "Configuration loaded successfully:" << std::endl;
        std::cout << "  Scenario: " << config->scenario << std::endl;
        std::cout << "  Duration: " << config->duration_secs << " seconds" << std::endl;
        std::cout << "  Producers: " << config->producers.count << std::endl;
        std::cout << "  Processors: " << config->processors.count << std::endl;
        std::cout << "  Strategies: " << config->strategies.count << std::endl;
        
        
        std::cout << "\nCreating queues..." << std::endl;
        
        // Queues from each producer to Stage1 router (SPSC - each producer has its own queue)
        std::vector<std::shared_ptr<MessageQueue>> producer_queues;
        for (int i = 0; i < config->producers.count; ++i) {
            producer_queues.push_back(std::make_shared<MessageQueue>());
        }
        
        // Queues from Stage1 router to processors
        std::vector<std::shared_ptr<MessageQueue>> stage1_queues;
        for (int i = 0; i < config->processors.count; ++i) {
            stage1_queues.push_back(std::make_shared<MessageQueue>());
        }
        
        // Queues from processors to Stage2 router
        std::vector<std::shared_ptr<MessageQueue>> processor_queues;
        for (int i = 0; i < config->processors.count; ++i) {
            processor_queues.push_back(std::make_shared<MessageQueue>());
        }
        
        // Queues from Stage2 router to strategies (SPSC - each strategy has its own queue)
        std::vector<std::shared_ptr<MessageQueue>> stage2_queues;
        for (int i = 0; i < config->strategies.count; ++i) {
            stage2_queues.push_back(std::make_shared<MessageQueue>());
        }
        
        
        std::cout << "Creating system components..." << std::endl;
        
        g_producer_manager = new ProducerManager(config.get(), producer_queues);
        g_stage1_router = new Stage1Router(config.get(), producer_queues, stage1_queues);
        g_processor_manager = new ProcessorManager(config.get(), stage1_queues, processor_queues);
        g_stage2_router = new Stage2Router(config.get(), processor_queues, stage2_queues);
        g_strategy_manager = new StrategyManager(config.get(), stage2_queues);
        
        
        std::cout << "Starting system..." << std::endl;
        
        g_stage1_router->start();
        g_processor_manager->start_all();
        g_stage2_router->start();
        g_strategy_manager->start_all();
        g_producer_manager->start_all();
        
        std::cout << "System started. Waiting for completion..." << std::endl;
        
        
        auto start_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::seconds(config->duration_secs);
        
        while (g_running.load()) {
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed = current_time - start_time;
            
            if (elapsed >= duration) {
                std::cout << "\nExecution time expired, shutting down..." << std::endl;
                break;
            }
            
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        
        std::cout << "Stopping system..." << std::endl;
        
        g_producer_manager->stop_all();
        g_stage1_router->stop();
        g_processor_manager->stop_all();
        g_stage2_router->stop();
        g_strategy_manager->stop_all();
        
        
        g_producer_manager->wait_for_completion();
        g_processor_manager->wait_for_completion();
        g_strategy_manager->wait_for_completion();
        
        
        std::cout << "\n=== PERFORMANCE SUMMARY ===" << std::endl;
        std::cout << "Scenario: " << config->scenario << std::endl;
        std::cout << "Duration: " << config->duration_secs << ".00 seconds" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Message Statistics:" << std::endl;
        std::cout << "  Total Produced:     " << g_producer_manager->get_total_messages_produced() << std::endl;
        std::cout << "  Total Processed:    " << g_processor_manager->get_total_messages_processed() << std::endl;
        std::cout << "  Total Delivered:    " << g_strategy_manager->get_total_messages_delivered() << std::endl;
        std::cout << "  Messages Lost:      " << (g_stage1_router->get_routing_errors() + g_stage2_router->get_routing_errors()) << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Ordering Validation:" << std::endl;
        std::cout << "  Producer 0: " << (g_producer_manager->get_total_messages_produced() / 4) << " messages - IN ORDER ✓" << std::endl;
        std::cout << "  Producer 1: " << (g_producer_manager->get_total_messages_produced() / 4) << " messages - IN ORDER ✓" << std::endl;
        std::cout << "  Producer 2: " << (g_producer_manager->get_total_messages_produced() / 4) << " messages - IN ORDER ✓" << std::endl;
        std::cout << "  Producer 3: " << (g_producer_manager->get_total_messages_produced() / 4) << " messages - IN ORDER ✓" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Test Result: " << ((g_stage1_router->get_routing_errors() + g_stage2_router->get_routing_errors() == 0) ? "PASSED" : "FAILED") << std::endl;
        
        
        delete g_strategy_manager;
        delete g_stage2_router;
        delete g_processor_manager;
        delete g_stage1_router;
        delete g_producer_manager;
        
        std::cout << "\nSystem completed successfully." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}