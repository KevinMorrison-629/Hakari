import { isAuthenticated } from './auth.js';
import { renderLoginView } from './views/loginView.js';
import { renderMainView } from './views/mainView.js';
import { renderRegisterView } from './views/registerView.js';

const appRoot = document.getElementById('app-root');

// This variable will track whether to show the 'login' or 'register' screen
let currentAuthView = 'login';

/**
 * Sets the authentication view ('login' or 'register') and triggers a re-render of the page.
 * @param {'login' | 'register'} viewName - The name of the view to display.
 */
export function setAuthView(viewName) {
    if (viewName === 'login' || viewName === 'register') {
        currentAuthView = viewName;
        // Call navigate to re-render with the correct auth view
        navigate();
    }
}

/**
 * Main navigation function. Clears the root element and renders the
 * appropriate view based on the user's authentication status and the
 * current auth view state.
 */
export function navigate() {
    if (!appRoot) {
        console.error("Fatal Error: #app-root element not found.");
        return;
    }

    // Clear the current view to prevent content from piling up
    appRoot.innerHTML = '';

    // If the user is authenticated, show them the main application
    if (isAuthenticated()) {
        renderMainView(appRoot);
    } else {
        // If not authenticated, decide whether to show the login or register view
        if (currentAuthView === 'register') {
            renderRegisterView(appRoot);
        } else {
            renderLoginView(appRoot);
        }
    }
}

// This is the initial call to render the correct view when the app first loads.
document.addEventListener('DOMContentLoaded', navigate);
