    if (relays_safe && soil_result == foundation::ErrorCode::Ok) {
        const foundation::ErrorCode farming_result = smart_farming.begin();
#ifdef ARDUINO
        Serial.print("[SMART-FARMING] begin ErrorCode=");
        Serial.println(static_cast<unsigned int>(farming_result));
#endif
        if (farming_result != foundation::ErrorCode::Ok) {
            logger.error("Smart Farming initialization FAILED");
        }
    } else {
        logger.error("Smart Farming initialization skipped because prerequisites failed");
    }

    logger.info("OpenIoT Logger setup validation");