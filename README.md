<a name="readme-top"></a>

<!-- GENERAL GAME INFO -->
<br />
<div align="center">

  <h2 align="center">Ghosts'n Goblins</h2>

  <p align="center">
  Ghosts 'n Goblins, known in Japan as Makaimura (Japanese: 魔界村, lit. "Demon World Village"), is a series of run-and-gun platform video games developed and owned by Capcom. The first entry in the series was Ghosts 'n Goblins, released in arcades on July 7, 1985. The series has subsequently been ported to and released on a variety of personal computers, game consoles and mobile platforms and spawned several sequels and spin-offs.

The main series focuses on the knight Arthur's quest to save princess Prin-Prin from the demon king Astaroth.
    <br />
    <strong>Original game : </strong>
    <a href="https://en.wikipedia.org/wiki/Ghosts_%27n_Goblins_(video_game)"><strong>General info »</strong></a>
    ·
    <a href="https://www.youtube.com/watch?v=P1VwMYwp80w"><strong>Youtube video »<strong></a>
    <br />
    <br />
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#my-version">My version</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
    </li>
    <li><a href="#how-to-play">How To Play</a></li>
    <li><a href="#class-structure">Class structure</a></li>
    <li><a href="#checklist">Checklist</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project
<img width="814" alt="gag" src="https://github.com/user-attachments/assets/f9e3d62d-e236-47d1-991c-f87f683f30b9" />


Here's why:

* I think this game is fun but also challenging.
* It allows me to prove my programming skills. 

<p align="right">(<a href="#readme-top">back to top</a>)</p>


## My version

This section gives a clear and detailed overview of which parts of the original game I planned to make.

### The minimum I will most certainly develop:
* player character animations
* camera movement
* moving platform
* water (the character dies if he falls there)
* ladder (climbing mechanic)
* 3 kinds of enemies
  - zombie
  - plant shooter
  - bird
* weapon (to be able to kill the enemies)
* boss fight in the end
* 3 kinds of pickups (that give different amount of score points)
  - coin (200), bag with money (500), ball (10 000)
* boss reward- key to open the door in the end
* UI:
  - Score counter
  - Timer

### What I will probably make as well:
* flame effect when the enemy is killed 
* pickups that have 10% to come out of enemies when they are killed, 10% chance to drop (again by chance 50/50 - monkey or necklace (200pts))
* the rest of the enemies
  - purple shield bearers
  - red demon
  - beige flying shooters

### What I plan to create if I have enough time left:
* second weapon (pickup from enemy)
* to collect the highest score in a json file, so every time the game loads, the highest score shows

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started
Detailed instructions on how to run your game project are in this section.

### Prerequisites

This is an example of how to list things you need to use the software and how to install them.
* Visual Studio 2022

### How to run the project
via Visual Studio (for the first time):
1. Open (DAE13_Petrova_Iva) folder
2. Open (DAE13_Petrova_Iva.sln)
3. Set GhostAndGoblins as Startup Project
4. Run it and have fun!

via .exe
1. Open (DAE13_Petrova_Iva) folder
2. Open (x64) folder
3. Open (xDebug)
4. Find (GhostsAndGoblins.exe)
5. Run it and have fun!



<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- HOW TO PLAY -->
## How to play

Use this space to show useful examples of how a game can be played. 
Additional screenshots and demos work well in this space. 

### Controls
* movement
  - A - left
  - D - right
  - W - climb up
  - S - crouch / climb down
* jump - SPACE
* shoot - E 

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CLASS STRUCTURE -->
## Class structure 

### Object composition 
I applied it for my bullets in Player, Plant, Devil and Boss classes.
* Player: 
  - creation: creates the PlayerBullet when the Player shoots (in Player::Update->Move)
  - update and deletion: in the Player::Update->HandleCollision->CheckLifeTimeBullets. This function calls the PlayerBullet::Update for each bullet and handles its collision with level/enemies. If the bullet collides with something, it returns a bool to the Player::CheckLifeTimeBullets and deletes it.
* Plant:
  - creation: creates the EnemyBullet in the Plant::Update->Shoot
  - update and deletion: in Plant::Update->UpdateBullets. This function calls the Update function for each enemy bullet and handles its collision inside the EnemyBullet class. If the bullet collides with the player, it returns a bool to the Plant::Update->UpdateBullets function and deletes it.
* Devil and Boss: 
  - creation: creates the EnemyBullet in the Boss::Update->HandleRandomAction->Shoot
  - update and deletion: (same as Plant)

### Aggregation:
I applied it in my Boss class.
  - I request the level vertices in my Boss constructor to use them in my collision handling in Boss::Update.
Why not pass it as a parameter in Update? Because Boss inherits from the base Enemy class which does not need the vertices for its Update method (the rest of the enemies don't need level collision). So, to handle the level collision for Boss specifically, I pass it to the constructor and "use" it.

### Association
I applied it in my Enemy class to create an EnemyCollectable.
Enemy has a method (DropCollectable()). It returns a Collectable pointer which points to a EnemyCollectable class. If an enemy (not boss) dies, it calls this method in the EnemyManager class and that method decides by chance if it should create an EnemyCollectable. If it does, it pushes it to the collectables vector (which is created in Game class).

### Inheritance 
I applied inheritance for my collectables and enemies.
* collectables- base class Collectable
  - BallCollectable
  - CoinCollecatble
  - MoneyBag
  - KeyCollectable
  - EnemyCollectable

* enemies: base class Enemy
  - Zombie
  - Bird
  - Plant
  - Boss
  - Shield Bearer
  - Devil


<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CHECKLIST -->
## Checklist

- [x] Accept / set up github project
- [ ] week 02 topics applied
    - [x] const keyword applied proactively (variables, functions,..)
    - [ ] static keyword applied proactively (class variables, static functions,..)
    - [x] object composition (optional)
- [x] week 03 topics applied (aggregation, association, inheritance)
- [x] week 04 topics applied (polymorphism, pure virtual, virtual, override, typecasting)
- [ ] week 05 topics applied (friend, operator overloading)
- [x] week 06 topics applied (explicit, rule of 3)
- [x] week 07 topics applied (l-references)
- [x] week 08 topics applied (rule of 5)
- [ ] week 09 topics applied (optional) (streams)
- [ ] week 10 topics applied (optional) (error handling)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

iva.petrova@student.howest.be

Project Link: [https://github.com/HowestDAE/dae13-ivappetrova](https://github.com/HowestDAE/dae13-ivappetrova)

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

Use this space to list resources you find helpful and would like to give credit to. 

* [Example 1: cpp reference on std::vector](https://en.cppreference.com/w/cpp/container/vector)
* [The site where I playtested the game](https://online.oldgames.sk/play/arcade/ghosts-n-goblins/10197)

<p align="right">(<a href="#readme-top">back to top</a>)</p>