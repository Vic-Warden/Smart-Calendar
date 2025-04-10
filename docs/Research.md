# Research to Market

**Introduction / Context :**

The daily use of calendars and reminder tools is often repetitive and not very engaging. To respond to this monotony, this research is part of a functional and experiential improvement of a connected calendar inspired by the Warhammer 40,000 universe. This project aims to integrate visual and audio interactive elements to enhance user engagement and offer an immersive experience.

The main target of this project is fans of the Warhammer 40,000 universe, a global community estimated between 30 and 50 million people. The dense and rich narrative universe of Warhammer allows for aesthetic and functional integration into everyday objects. The project thus proposes an original alternative to the standard reminder: personalized voice interactions, thematic aesthetics, and automatic audio activations triggered by user presence.

---

**Research question :**

What front-end tools are currently available ?

**Sub-questions :**

How can the chosen tool be used to integrate interactive elements easly ?

---

**Research method :**

To answer the research question, a fairly simple method was used: an online literature review to compare the best-known front-end development tools. The goal was to find tools that could improve a website's visual style and allow the addition of interactions such as sound or animation.

The steps were as follows:

First, list the most commonly used tools such as React, Vue, Angular, Vanilla JS, or Svelte

Then, check their official documentation to see what features they offer

Read recent comparative articles written by developers to better understand the pros and cons of each

Check whether they make it easy to add styles, sounds, animations, or visual effects

Finally, look at how easy these tools are to use and how well they are explained online

Even if this method is not complex, it gives a clear and honest overview of what exists today to improve a modern and dynamic web interface.

---

**Results**

The research helped to better understand what each tool offers. An article titled [Comparing Angular, React, Vue, and Vanilla-JS](https://dev.to/rfornal/comparing-angular-react-vue-and-vanilla-js-37o9)  helped sort out the best-known frameworks.

React is very complete, well-documented, with lots of tutorials and a large community. It also allows you to easily add animations, sound, or visual effects.

Vue.js is easier to start with, but the community is slightly smaller, which can sometimes make finding help a bit harder.

Angular is very powerful but mostly used in large professional projects. It’s a bit more complicated to learn.

Vanilla JS is useful for small tasks, but not sufficient to handle a visually advanced project.

Another article, [Introducing Svelte, and Comparing Svelte with React and Vue](https://joshcollinsworth.com/blog/introducing-svelte-comparing-with-react-vue), presents Svelte as a simple, efficient, and fast-to-use tool. It requires little code, is close to the HTML/CSS we already know, and simplifies component management. Its community is smaller than React’s, but this does not pose much of a problem for this type of project.

---

**Implementation**

After choosing which tool I was going to use to improve my front-end, I thought long and hard about what I could improve or add. I reread my website and asked myself, if I were a person who knew nothing about Warhammer 40k, what elements would I like to have for a little more explanation ? I first wrote my explanatory text to find out more about Omnissiah and techpriests. Then I was thinking, okay, I've got the text, but how do I add my text to my website? I started thinking, well, in Space Marine 2, in the campaign, there's a computer that explains the purpose of our first mission, and I really liked the animations on it, except that I'm not a professional developer, so I made it look like it's an Astra Militarum terminal, I had to add a few more lines to make it look like someone was trying to connect to it, so I'll let you see what I've implemented in the video.

**Proof 1**: [See the first proof](./proof1.mp4)  


Concerning the 2nd page, I thought that everything was more or less explained, so why not concentrate on my 3D model? It's true that when you look at my 3D model, you can ask yourself various questions: what does the PIR sensor, the button and the photoresistor do? That's why I've explained everything, and what's more, I've succeeded with great difficulty in implementing my 3D model with the ability to rotate it to show it from all angles ! 


**Proof 2**: [See the second proof](./proof2.mp4)  

---

**Code**

Display the terminal with Svelte :

```html
<div id="omnissiah-overlay" class="hidden fixed inset-0 bg-black bg-opacity-90 backdrop-blur-sm z-50 flex items-center justify-center">
  <div class="terminal_svelte text-green-400 font-mono p-6 rounded-lg max-w-xl w-full mx-4 border border-green-500 shadow-xl animate-popup relative overflow-hidden">
    ...
  </div>
</div>
```

Display the terminal without Svelte :

```html
<!-- HTML -->
<div id="omnissiah-overlay" class="hidden fixed inset-0 bg-black bg-opacity-90 backdrop-blur-sm z-50 flex items-center justify-center">
  <div class="text-green-400 font-mono p-6 rounded-lg max-w-xl w-full mx-4 border border-green-500 shadow-xl relative overflow-hidden">
    
    <!-- Spinning gear icon -->
    <div class="absolute top-4 right-6 animate-spin text-green-500">⚙️</div>

    <!-- Terminal heading -->
    <h2 class="text-lg font-bold mb-4">[ Astra Militarum Transmission Terminal ]</h2>

    <!-- Dynamic terminal text output -->
    <div id="terminal-text" class="whitespace-pre-line text-green-300 text-sm leading-relaxed">
      Accessing data logs... \n Omnissiah protocol initiated...
    </div>
    <span class="inline-block w-2 h-4 bg-green-400 animate-pulse mt-2"></span>

    <!-- Terminal loading bar -->
    <div id="terminal-loading" class="h-1 bg-green-500 mt-6 rounded-full w-0"></div>

    <!-- Close button -->
    <div class="text-right mt-6">
      <button onclick="closeOverlay()" class="bg-green-700 hover:bg-green-600 text-white py-2 px-4 rounded">OK</button>
    </div>
  </div>
</div>

<!-- Button to toggle terminal -->
<button onclick="openOverlay()" class="fixed bottom-6 right-6 bg-red-900 text-white px-4 py-2 rounded z-50">Who is the Omnissiah ?</button>

<!-- JS -->
<script>
  function openOverlay() {
    document.getElementById("omnissiah-overlay").classList.remove("hidden");
    document.getElementById("terminal-loading").style.width = "100%";
  }

  function closeOverlay() {
    document.getElementById("omnissiah-overlay").classList.add("hidden");
    document.getElementById("terminal-loading").style.width = "0";
  }
</script>
```

Display the 3D model and the Animation text with Svelte :

```svelte
<script>
  import { onMount } from 'svelte';
  let visible = false;

  let text = 'Tactical interface loaded.\nOmnissiah protocol active.';
  let display = '';
  let i = 0;

  function toggleViewer() {
    visible = !visible;
    if (visible) {
      display = '';
      i = 0;
      decrypt();
    }
  }

  function decrypt() {
    const interval = setInterval(() => {
      if (i < text.length) {
        display += text[i];
        i++;
      } else {
        clearInterval(interval);
      }
    }, 35);
  }

  onMount(() => {
    // Three.js init can go here too if needed
  });
</script>

<button on:click={toggleViewer}>Inspect Model</button>

{#if visible}
  <div>
    <div id="model-viewer" style="width:500px;height:500px;"></div>
    <pre>{display}</pre>
    <button on:click={toggleViewer}>Close</button>
  </div>
{/if}

```

Display the 3D model and the Animation text without Svelte :

```html
<!-- BOUTON POUR LANCER LE MODÈLE 3D -->
<button onclick="openViewer()">Inspect Model</button>

<!-- CONTAINER 3D + TEXTE CRYPTÉ -->
<div id="viewer" style="display:none;">
  <div id="model-viewer" style="width:500px;height:500px;"></div>

  <div class="decrypt-text" data-text="Tactical interface loaded.\nOmnissiah protocol active."></div>

  <button onclick="closeViewer()">Close</button>
</div>

<!-- JS DE BASE -->
<script src="https://cdn.jsdelivr.net/npm/three@0.141.0/build/three.min.js"></script>
<script src="https://cdn.jsdelivr.net/npm/three@0.141.0/examples/js/loaders/STLLoader.js"></script>

<script>
  let scene, camera, renderer, mesh;

  function openViewer() {
    document.getElementById('viewer').style.display = 'block';
    initViewer();
    startDecryptAnimation();
  }

  function closeViewer() {
    document.getElementById('viewer').style.display = 'none';
  }

  function initViewer() {
    const container = document.getElementById('model-viewer');
    container.innerHTML = '';

    scene = new THREE.Scene();
    camera = new THREE.PerspectiveCamera(75, 1, 0.1, 1000);
    camera.position.set(100, 100, 100);
    camera.lookAt(0, 0, 0);

    renderer = new THREE.WebGLRenderer({ alpha: true });
    renderer.setSize(500, 500);
    container.appendChild(renderer.domElement);

    const loader = new THREE.STLLoader();
    loader.load('/models/your_model.stl', function (geometry) {
      const material = new THREE.MeshNormalMaterial();
      mesh = new THREE.Mesh(geometry, material);
      mesh.rotation.set(-Math.PI / 2, 0, 0);
      scene.add(mesh);
    });

    animate();
  }

  function animate() {
    requestAnimationFrame(animate);
    if (mesh) mesh.rotation.z += 0.005;
    renderer.render(scene, camera);
  }

  function startDecryptAnimation() {
    const el = document.querySelector('.decrypt-text');
    const original = el.dataset.text;
    let display = Array.from(original).map(() => ' ');
    let i = 0;
    el.style.opacity = 1;

    const interval = setInterval(() => {
      for (let j = i; j < original.length; j++) {
        display[j] = Math.random() > 0.5 ? String.fromCharCode(33 + Math.random() * 94) : display[j];
      }
      el.innerText = display.join('');
      display[i] = original[i];
      i++;
      if (i >= original.length) {
        el.innerText = original;
        clearInterval(interval);
      }
    }, 35);
  }
</script>

```

---

**Conclusion**

In the end, even though React is very complete and widely used in development, Svelte seems more suitable for a personal or educational project. It is easier to use, quicker to set up, and allows for adding animations or styling without too much code. For this project, Svelte is a good match for the needs.

---

**Recommandations**

Next time, we could try using Svelte with other tools like GSAP, since that might help make the animations cleaner. Also, it would be good to think about making the site more accessible, like adapting better to small screens or using proper ARIA features. And if possible, find a way so even people who don’t code can add effects or content without struggling.

---

**Sources and References**

[Comparing Angular, React, Vue, and Vanilla-JS](https://dev.to/rfornal/comparing-angular-react-vue-and-vanilla-js-37o9)

[Svelte vs React Comparison](https://dev.to/rfornal/comparing-angular-react-vue-and-vanilla-js-37o9)

[Introducing Svelte, and Comparing Svelte with React and Vue](https://joshcollinsworth.com/blog/introducing-svelte-comparing-with-react-vue)