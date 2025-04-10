# Technical Documentation

Write here your own content!


## Design Decision

For this project, I wanted to bring my personal touch to my favorite universe, Warhammer 40k! Although this universe is extremely vast, I decided to focus on the cult of Omnissiah, with one thing in mind: ease of use. My website is composed of two main pages:

An introductory page index.html with a terminal for more information.

A main page inside_spirit_machine.html dedicated to adding, modifying and deleting appointments and its 3D model.

## Problems Encountered & Solutions

When I got to my introduction page, not all my elements had time to load properly - well, they did load, but not every time, so to make sure all my elements had time to load properly I set up a loading screen with an animation that simulated the awakening of Machine Spirit. Then all the content was displayed with a transition effect.

Some elements of the introduction page, such as the introduction terminal, are managed in JS, and the " Who is the Omnissiah? " button is supposed to open an oerlay terminal. The problem is that the code was looking for the #omnissiahButton in my html file and the ID is toggle-button. As a result, the getElementByI returned null, which caused a block. To avoid this, I corrected the timing by putting everything in a document.addEventListener function.

## Technologies Used

My website relies on HTML files for its structure, CSS for styling and JS for interaction. And PHP to interact with my database. This allows me to better structure my code, and since this is the convention, I might as well kill two birds with one stone.

As far as CSS is concerned, I've used CSS that lets me manage animations, for example, and Tailwind CSS to save time when styling my buttons, for example, instead of putting it in my script.js I can directly change the color, size and even the text directly in my html.

As far as JavaScript is concerned, it allows me to manage my terminal on my home page (e.g. line by line writing). The appointment creation form, dynamic data loading and display, appearance animations, loading and integration with the 3D model via Three.js

On the PHP side, it lets me add insert_appointment.php, modify update_appointment.php, delete delete_appointment.php and recover appointments_appointment.php. I used it because it's easy to use and, above all, because it works directly with phpMyAdmin.

Concerning MySQL, it allows me to store my appointments, it contains different tables that I use for my project but for my website I use the Appointment table, which allows me to add the task, date and time. 

Concerning Tree.js, it allows me to display my 3D model thanks to the Tree.js library. With the STLLoader module, I was able to load my stl file and display it when the user pressed the " Tactical Model Inspection " button.

## Structure of the Website

My site is divided into two main parts:

The index.html home page introduces the world of the site. It contains an interactive terminal, animated line by line, which the user can open by clicking on the “Who is the Omnissiah?” button. The purpose of this page is to explain which blueprint I've used, and for those unfamiliar with the Warhammer 40k universe to understand who the Omnissiah is.

The inside_spirit_machine.html main page is where it all happens. The user can :

Add, Modify, Delete, See appointments dynamically displayed and Launch a 3D visualization of an Astra Militarum model.All data is managed via fetch calls to a PHP API, so I don't have to reload the page for each interaction. 

## Test

To make sure everything was working perfectly, I manually checked that every button was working, that every interaction was running smoothly, and for the API test I wrote a PHP script to check that my appointments were present in my database. Once the script was finished, I just had to type this link on my search bar 

http://localhost/Database/Appointment/recover_appointment.php

## Deployment

I used Docker to manage my local environment. This allows me to launch my PHP files, test my queries, and interact with my MySQL database with phpMyAdmin accessible from localhost.

## Target Audience 

This project has obviously been designed for fans of the Warhammer 40k universe, and I've chosen a techno-religious style to stay true to the Techpriest. The terminal, the different colors, the chosen images and even the 3D model are all related to Warhammer 40k, BUT I haven't forgotten all the people who don't know Warhammer 40k at all, thanks to my terminal, which will help them better understand the whys and wherefores.

## Contemporary Developments

I used fetch API calls, Tailwind CSS, I separated my files into HTML, CSS, JS, PHP, Tree.js to integrate a 3D model into the browser.