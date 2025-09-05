import { isAuthenticated } from './auth.js';
import { renderLoginView } from './views/loginView.js';
import { renderMainView } from './views/mainView.js';

const appRoot = document.getElementById('app-root');

/**
 * Main navigation function. Clears the root element and renders the
 * appropriate view based on the user's authentication status.
 */
export function navigate() {
    if (!appRoot) {
        console.error("Fatal Error: #app-root element not found.");
        return;
    }

    // Clear the current view
    appRoot.innerHTML = '';

    // Render the correct view
    if (isAuthenticated()) {
        renderMainView(appRoot);
    } else {
        renderLoginView(appRoot);
    }
}

// Initial call to render the correct view when the app loads.
document.addEventListener('DOMContentLoaded', navigate);

