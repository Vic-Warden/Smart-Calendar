document.addEventListener("DOMContentLoaded", function() {
    setTimeout(() => {
        let loadingScreen = document.getElementById("loading-screen");
        loadingScreen.style.transition = "opacity 1s ease-out";
        loadingScreen.style.opacity = "0";

        setTimeout(() => {
            loadingScreen.style.display = "none";
            document.getElementById("main-content").style.display = "block";
        }, 1000);
    }, 3800);
});
