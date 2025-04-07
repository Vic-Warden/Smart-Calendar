# Research to Market

**Introduction / Context :**

When we need to add a task to our schedule, whether in a calendar or on our phone, we always do the same thing, and this can quickly become monotonous. To make this task more attractive, my aim was to transform the traditional cuckoo clock and reminder into a fully personalized smart calendar. In concrete terms, users will be able to enter their appointments via a website which, of course, is calendar-themed in the Warhammer 40,000 universe. This will enable them to organize their daily activities.

Who is this project aimed at? Of course, we're talking about fans of the Warhammer 40,000 universe, which boasts a community of between 30 and 50 million people. Just to give you an idea, let's say that a video game that has been awaited for 13 years has managed to reach 5 million sales. I'd like to remind you that just for one game among many others, 5 million people installed the game and studied the Warhammer lore. To tell you the Warhammer 40,000 lore comprises 1,097 works, including novels, short stories and collections. I'll stop talking about Warhammer 40,000, but you can see how well-developed and extremely well-constructed this universe is. My main idea was to integrate this universe in an immersive and captivating way! For an everyday experience, for example, instead of the traditional hourly birdsong, the user will hear an emblematic commissar's voice line, taken directly from the Dawn of War game series. To remain faithful to my favorite games, I wanted to take up its principle, so that every interaction, such as adding an appointment or simply modifying one, becomes a unique experience.

A concrete scenario: Imagine a user standing in front of the revisited smart calendar. Upon detecting his presence, the eyes of the Astra Militarum emblem one of the most emblematic factions in Warhammer 40,000 light up red, accompanied by a vocal cue, but this time it's not the commissar's voice you hear, but the machine's spirit speaking directly to you! This immersive setting aims to add a touch of detail, making everyday use of the smart calendar entertaining. 

What's more, when creating my website, I really wanted to make it as easy to use as possible. My main objective was to make it quick and easy to add an appointment. This ensured that my smart calendar would be most attractive to a wide range of users, from Warhammer enthusiasts to casual users and even the curious in search of new experiences. The aesthetic and sound aspects of my project, inspired by the world of Warhammer 40,000. allows me not only to add sentimental value for enthusiasts, but also to arouse the interest of a wider public, intrigued by novelty and originality. 

The last but not least, to enhance the user experience, the improvement I had in mind would be to add to the web interface a personalized choice of voice unit associated with each appointment. In this way, the user would no longer be limited to the curator's choices, but could select voice lines for iconic characters such as the Psyker, the Priest, the Assassin, or even iconic vehicles such as the Baneblade or the Leman Russ. Making every interaction as personalized as possible !

---

**Research question :**

Which tools can most easily help improve the style of a website’s front-end  ?

**Sub-questions :**

How can the chosen tool be used to integrate interactive elements ?

---

**Research method :**

As for my research, I don't know if this “research technique” has a name because I'm not going to lie to you, sir. Your part is the part I dread the most, I'm not taking too much risk in telling you that we French students this is a first for us, so before doing any kind of research, I took out my phone and started writing down what I really want to do with this upgrade. I only need 2 things to choose the tools

- The first was, of course, the possibility of enhancing the visual

- if the chosen tool could allow me to add audio, animation and even icons. 

So I started by searching the Internet for tools that met my two criteria. I noticed that there were often some tools that came up most often, so I looked into them a little more. React, Vue.js, Angular, and of course, Vanilla JS. So I decided to dig a little deeper into each of them.

After a long research session I came across an article [Comparing Angular, React, Vue, and Vanilla-JS](https://dev.to/rfornal/comparing-angular-react-vue-and-vanilla-js-37o9) that compared different tools I was planning to use. Although I didn't know which one I should take, I decided to look it up and take only the information that was important to me. 

Thanks to this article, I was able to learn quite a few things:

- For example, regarding React, the author writes:

" React has suitable testing tools. Their documentation just got a major upgrade and their community can be best described as 'fierce'. "

I know that thanks to this there's documentation and I know that at first glance it's not something you look at first, but I'd already encountered a similar problem because when I had to install a mod for my favorite game, Dawn of War, I was quite happy to have access to a panoply of documentation that allowed me to solve all the problems I'd encountered, and without that I know I'd be unable to play it. So for me, having documentation is an important thing.

- Regarding Vanilla, he explains that:

" Vanilla JavaScript is not a good solution for larger endeavors. "

Even if Vanilla is easy, it quickly becomes difficult to use for larger endeavors, so I don't think I'll be using it.

- Regarding Angular, he explains that:

" Angular is geared to Test Driven Development and is well suited for Enterprise level development. Their documentation is excellent and the community is responsive. "

I can see that it's a development tool, but I'm not doing a professional project... I'm doing a personal project first and foremost, so I'm not going to choose it either!

- For Vue, the author also says:

" The community is small but this can work as an advantage since changes don't come out as frequently as Angular or React; requiring less general updating of the code. "

It's obvious that it's a tool that's easy to use, but the problem is the lack of community and the lack of documentation. I've encountered too many problems to leave that point.

It's not about one tool in particular, but thanks to it, I know that there isn't just one tool that can answer my question, but that there are many that can.

" Many of the popular libraries are now more comparable than ever which allows organizations to truly select which pattern(s) work best for them. "

After reading this article my preferred choice is React because it has good documentation, the ability to do many things including animations, add icons and even sounds.

Then I read an article [Svelte vs React Comparison](https://dev.to/rfornal/comparing-angular-react-vue-and-vanilla-js-37o9) that I found interesting, which directly compared Svelte and React. 

" Svelte cuts lots of corners in its compilation. It interprets its application code during build time. "

This shows that Svelte does some of the work at compile time, making it faster and smoother.

He also explains that :

" Reactjs has a larger community and more resources for developers. There are tons upon tons of React blogs treating various features and advanced topics of Reactjs. "

That's a decisive factor for me: having a large community means more tutorials, more answers if you get stuck, and more accessible resources to help you progress.

In the article [Introducing Svelte, and Comparing Svelte with React and Vue](https://joshcollinsworth.com/blog/introducing-svelte-comparing-with-react-vue) I read that he said that 

" From the start, I was consistently and pleasantly surprised how little code I needed to do things in Svelte-and how close it was to the JavaScript, HTML and CSS I already knew. "

And that's exatly how I feel, I'm not a pro developer, I just want to improve my project without learning a super complicated trick, and Svelte makes it possible to do that, it even goes further saying “Svelte is React without all the bullshit.” It's a bit direct, but I understand what he means, simple and fast. 

He also explains that Svelte avoids some common complications, particularly concerning component state, stating that:

" State management is drastically simpler in Svelte compared to React or Vue. "

This confirms the idea that Svelte simplifies things enormously, which is exactly what I'm looking for.

---

**Implementation**

After choosing which tool I was going to use to improve my front-end, I thought long and hard about what I could improve or add. I reread my website and asked myself, if I were a person who knew nothing about Warhammer 40k, what elements would I like to have for a little more explanation ? I first wrote my explanatory text to find out more about Omnissiah and techpriests. Then I was thinking, okay, I've got the text, but how do I add my text to my website? I started thinking, well, in Space Marine 2, in the campaign, there's a computer that explains the purpose of our first mission, and I really liked the animations on it, except that I'm not a professional developer, so I made it look like it's an Astra Militarum terminal, I had to add a few more lines to make it look like someone was trying to connect to it, so I'll let you see what I've implemented in the video.

**Proof 1**: [See the first proof](./proof1.mp4)  


Concerning the 2nd page, I thought that everything was more or less explained, so why not concentrate on my 3D model? It's true that when you look at my 3D model, you can ask yourself various questions: what does the PIR sensor, the button and the photoresistor do? That's why I've explained everything, and what's more, I've succeeded with great difficulty in implementing my 3D model with the ability to rotate it to show it from all angles ! 


**Proof 2**: [See the second proof](./proof2.mp4)  

---

**Conclusion**

After looking at different tools and comparing their advantages and disadvantages through various articles, I realize that React is clearly the most powerful tool, it has very complete documentation and a community that is clearly active. That's a real plus, but what I'm looking for above all isn't the most powerful tool, it's a tool that lets me get down to basics. So yes, I admit that thanks to my research I've managed to come to the same conclusion as Yanis, who also agrees that React is an excellent tool, but Yanis is a web developer, he's simply excellent, and I'm clearly not on his level, which is why my solution will be different from yours. And as the article says 

" From the start, I was consistently and pleasantly surprised how little code I needed to do things in Svelte-and how close it was to the JavaScript, HTML and CSS I already knew. "

I'm not a professional developer, I just want to enhance my project with some visuals and animations. And Svelte simplifies things enormously. 

That's why I decided to use Svelte for my implementation !


---

**Sources and References**

[Comparing Angular, React, Vue, and Vanilla-JS](https://dev.to/rfornal/comparing-angular-react-vue-and-vanilla-js-37o9)

[Svelte vs React Comparison](https://dev.to/rfornal/comparing-angular-react-vue-and-vanilla-js-37o9)

[Introducing Svelte, and Comparing Svelte with React and Vue](https://joshcollinsworth.com/blog/introducing-svelte-comparing-with-react-vue)