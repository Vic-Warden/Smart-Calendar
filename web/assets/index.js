class App 
{
  constructor() 
  {
    const toggleButton = document.getElementById("toggle-button");
    const overlay = document.getElementById("omnissiah-overlay");
    const terminalText = document.getElementById("terminal-text");
    const terminalLoading = document.getElementById("terminal-loading");
    const buttonWrapper = document.getElementById("omnissiah-button-wrapper");

    let loadingInterval, typingInterval, timeoutNextLine;
    let isClosed = false;

    if (!toggleButton || !overlay || !terminalText || !terminalLoading) 
      {
        console.warn("Un ou plusieurs éléments nécessaires sont manquants.");
        return;
    }

    toggleButton.addEventListener('click', () => 
      {
      if (buttonWrapper) 
        {
          buttonWrapper.style.display = 'none';
        }

      overlay.classList.remove("hidden");
      terminalText.textContent = '';
      terminalLoading.style.width = '0%';
      isClosed = false;

      let progress = 0;
      clearInterval(loadingInterval);
      clearInterval(typingInterval);
      clearTimeout(timeoutNextLine);

      loadingInterval = setInterval(() => 
        {
          progress += 10;
          terminalLoading.style.width = `${progress}%`;

          if (progress >= 100) 
            {
              clearInterval(loadingInterval);

              const textLines = 
              [
                "Connection to Astra Militarum terminal...",
                "Established interface",
                `Omnissiah is revered as the Machine-God, source of all knowledge and mechanical perfection. 
                The Cult Mechanicus, in its sacred zeal, considers every circuit board a litany, every motor a divine choir.
                Every machine, whether a simple servo-skull or a titan of war, harbors a Machine Spirit a mysterious and sensitive entity, both benevolent and capricious.
                Techno-Priests, guardians of forgotten secrets and ancient rites, perform codified rituals: a mix of precise gestures, binary prayers and the manipulation of electro-purifying incense burners.
                Through these rites, they awaken the machine spirits, imploring their cooperation and power.
                Their unshakeable faith guides every incantation, every energy impulse, like an offering to the great Source Spirit.
                For where the ignorant see an assembly of metal, they perceive a soul, a consciousness encapsulated in the sacred gears.
                [ Transmission ends ]`
            ];
            

            typeLineByLine(textLines, terminalText, 30, 1200);
        }
      }, 150);
    });

    document.addEventListener("click", (e) => 
      {
      if (e.target && e.target.id === "close-overlay") 
        {
          overlay.classList.add("hidden");
          terminalText.textContent = '';
          terminalLoading.style.width = '0%';

        if (buttonWrapper) 
          {
            buttonWrapper.style.display = 'block';
          }

          isClosed = true;
          clearInterval(loadingInterval);
          clearInterval(typingInterval);
          clearTimeout(timeoutNextLine);
      }
    });

    function typeLineByLine(lines, el, delay = 30, lineDelay = 1200) 
    {
      let i = 0;

      function nextLine() 
      {
        if (isClosed || i >= lines.length) return;

        el.textContent = '';
        let line = lines[i];
        let j = 0;

        clearInterval(typingInterval);

        typingInterval = setInterval(() => 
          {
            if (isClosed) 
              {
                clearInterval(typingInterval);
                return;
             }

          el.textContent += line.charAt(j);
          j++;

          if (j >= line.length) 
            {
             clearInterval(typingInterval);
             i++;
             timeoutNextLine = setTimeout(nextLine, lineDelay);
           }
        }, delay);
      }

      nextLine();
    }
  }
}

export default App;