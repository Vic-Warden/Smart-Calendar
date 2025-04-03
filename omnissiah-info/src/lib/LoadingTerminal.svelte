<script>
  import { onMount } from 'svelte';

  let lines = 
  [
    '>> SYSTEM BOOT SEQUENCE INITIATED',
    '>> POWER CORE STABILIZING... OK',
    '>> RITUAL PRAYER TO THE OMNISSIAH... OK',
    '>> ASTRAMIL CORE BIOS v3.3.7 LOADED',
    '>> LINKING TO MUNITORUM NET... ESTABLISHED',
    '>> ACCESSING STC FRAGMENTS... DECRYPTING...',
    '>> WAKING THE MACHINE SPIRIT...'
  ];

  let displayLines = [];
  let index = 0;

  onMount(() => 
  {
    const bootSound = new Audio('/sounds/boot.mp3');
    bootSound.volume = 0.4;
    bootSound.play();
    nextLine();
  });

  function nextLine() 
  {
    if (index < lines.length) {
      displayLines = [...displayLines, lines[index++]];
      setTimeout(nextLine, 800);
    }
  }
</script>

<style>
  .terminal-background 
  {
    position: fixed;
    top: 0; left: 0;
    width: 100%; height: 100vh;
    background: #000;
    color: #00ff99;
    font-family: 'Share Tech Mono', monospace;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    z-index: 9999;
    overflow: hidden;
  }

  .crt-lines::after 
  {
    content: "";
    position: absolute;
    top: 0; left: 0;
    width: 100%; height: 100%;
    background: repeating-linear-gradient
    (
      0deg,
      rgba(0, 255, 0, 0.03) 0px,
      rgba(0, 255, 0, 0.03) 1px,
      transparent 1px,
      transparent 2px
    );
    pointer-events: none;
    z-index: 5;
    animation: flicker 0.3s infinite;
  }

  @keyframes flicker 
  {
    0%, 100% { opacity: 0.06; }
    50% { opacity: 0.12; }
  }

  .heartbeat 
  {
    width: 60px;
    height: 60px;
    margin-bottom: 20px;
  }

  .beat 
  {
    stroke: red;
    stroke-width: 3;
    fill: none;
    stroke-linecap: round;
    animation: beat 1.2s ease-in-out infinite;
  }

  @keyframes beat 
  {
    0%, 100% { transform: scale(1); opacity: 1; }
    50% { transform: scale(1.2); opacity: 0.8; }
  }
</style>

<div class="terminal-background crt-lines">
  <svg class="heartbeat" viewBox="0 0 100 100">
    <polyline class="beat" points="0,60 20,60 30,40 40,70 50,30 60,60 100,60" />
  </svg>
  <p>The Machine Spirit is awakening...</p>
  <div class="mt-4 text-left text-sm w-[90%] max-w-xl">
    {#each displayLines as line}
      <div>{line}</div>
    {/each}
  </div>
</div>
