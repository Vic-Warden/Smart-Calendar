-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Schema iot
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema iot
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `iot` DEFAULT CHARACTER SET utf8 ;
USE `iot` ;

-- -----------------------------------------------------
-- Table `iot`.`Device`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `iot`.`Device` ;

CREATE TABLE IF NOT EXISTS `iot`.`Device` (
  `device_id` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(20) NOT NULL,
  `ip_address` VARCHAR(15) NOT NULL,
  PRIMARY KEY (`device_id`),
  UNIQUE INDEX `ip_address_UNIQUE` (`ip_address` ASC) VISIBLE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `iot`.`Appointment`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `iot`.`Appointment` ;

CREATE TABLE IF NOT EXISTS `iot`.`Appointment` (
  `appointment_id` INT NOT NULL AUTO_INCREMENT,
  `task` VARCHAR(255) NOT NULL,
  `date_hour` DATETIME NOT NULL,
  `device_id` INT NOT NULL,
  PRIMARY KEY (`appointment_id`),
  INDEX `date_hour` (`date_hour` ASC) VISIBLE,
  INDEX `fk_appointment` (`device_id` ASC) VISIBLE,
  CONSTRAINT `fk_appointment`
    FOREIGN KEY (`device_id`)
    REFERENCES `iot`.`Device` (`device_id`)
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `iot`.`Sensor`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `iot`.`Sensor` ;

CREATE TABLE IF NOT EXISTS `iot`.`Sensor` (
  `sensor_id` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(45) NOT NULL,
  `type` ENUM('Presence', 'Light', 'Button') NOT NULL,
  `activation_threshold` FLOAT NULL,
  `device_id` INT NOT NULL,
  PRIMARY KEY (`sensor_id`),
  INDEX `type` (`type` ASC) VISIBLE,
  INDEX `fk_sensor` (`device_id` ASC) VISIBLE,
  CONSTRAINT `fk_sensor`
    FOREIGN KEY (`device_id`)
    REFERENCES `iot`.`Device` (`device_id`)
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `iot`.`SensorData`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `iot`.`SensorData` ;

CREATE TABLE IF NOT EXISTS `iot`.`SensorData` (
  `data_id` INT NOT NULL AUTO_INCREMENT,
  `value` FLOAT NOT NULL,
  `time_stamp` DATETIME NOT NULL,
  `button_state` TINYINT NULL DEFAULT 0,
  `sensor_id` INT NOT NULL,
  PRIMARY KEY (`data_id`),
  INDEX `timestamp` (`time_stamp` ASC) VISIBLE,
  INDEX `fk_data` (`sensor_id` ASC) VISIBLE,
  CONSTRAINT `fk_data`
    FOREIGN KEY (`sensor_id`)
    REFERENCES `iot`.`Sensor` (`sensor_id`)
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
