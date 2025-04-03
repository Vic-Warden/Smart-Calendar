<script>
    import { onMount } from 'svelte';
    import * as THREE from 'three';
    import { STLLoader } from 'three/examples/jsm/loaders/STLLoader.js';
  
    let container;
  
    onMount(() => 
    {
      const scene = new THREE.Scene();
      const camera = new THREE.PerspectiveCamera(75, 1, 0.1, 1000);
      const renderer = new THREE.WebGLRenderer({ alpha: true, antialias: true });
  
      renderer.setSize(300, 300);
      container.appendChild(renderer.domElement);
  
      const loader = new STLLoader();
      loader.load('/models/Astra_Militarum_Explication.stl', (geometry) => 
      {
        const material = new THREE.MeshStandardMaterial({ color: 0x00ff99 });
        const mesh = new THREE.Mesh(geometry, material);
        mesh.rotation.x = -Math.PI / 2;
        scene.add(mesh);
        camera.position.z = 100;
  
        const animate = () => 
        {
          requestAnimationFrame(animate);
          mesh.rotation.y += 0.01;
          renderer.render(scene, camera);
        };
  
        animate();
      });
  
      const light = new THREE.DirectionalLight(0xffffff, 1);
      light.position.set(0, 0, 100).normalize();
      scene.add(light);
    });
  </script>
  
  <style>
    .viewer-container 
    {
      width: 300px;
      height: 300px;
      background: radial-gradient(circle, #000000, #111111);
      border: 1px solid #00ff99;
      box-shadow: 0 0 12px rgba(0, 255, 153, 0.4);
      border-radius: 0.5rem;
    }
  
    .description 
    {
      max-width: 320px;
      margin-top: 1rem;
      font-size: 0.875rem;
      color: #99ffcc;
      font-family: 'Share Tech Mono', monospace;
      line-height: 1.5;
      text-align: center;
    }
  </style>
  
  <div class="flex flex-col items-center">
    <div class="viewer-container" bind:this={container}></div>
  
    <p class="description">
      Unlike the cowardly Eldar, who will freely run from a fight to regroup, the Imperial Guard can never retreat.
      Once committed to a fight, they either win or fight to the last man, and the Commissars ensure they will perform this duty
      whether they want to or not. Following this ethos, the Field Command is built out of immobile rockcrete with firing slits
      on all sides for last-ditch defense. Once deployed, it will either direct the Imperial Guard forces to victory or let them
      take as many of the enemy with them as possible.
    </p>
  </div>
  