import { login } from '../auth.js';
import { setAuthView } from '../main.js';
import { showNotification } from '../ui/notification.js';

/**
 * Renders the login page into a given container element.
 * @param {HTMLElement} container - The element to render the page in.
 */
export function renderLoginView(container) {
    container.innerHTML = `
        <div class="container-centered">
            <div class="card">
                <div class="card-header">
                    <h2>Welcome Back!</h2>
                    <p>Please sign in to your account.</p>
                </div>
                <form id="loginForm">
                    <div class="form-group">
                        <label for="email" class="form-label">Email</label>
                        <input type="email" id="email" class="form-input" placeholder="you@example.com" required>
                    </div>
                    <div class="form-group">
                        <label for="password" class="form-label">Password</label>
                        <input type="password" id="password" class="form-input" placeholder="••••••••" required>
                    </div>
                    <button type="submit" class="btn btn-primary">Sign In</button>
                    <button type="button" id="toggle-view-btn" class="btn btn-link">
                        Don't have an account? Sign Up
                    </button>
                </form>
            </div>
        </div>
    `;

    // --- Event Listeners ---
    const loginForm = container.querySelector('#loginForm');
    const submitButton = container.querySelector('button[type="submit"]');

    // Handle form submission
    loginForm.addEventListener('submit', async (event) => {
        event.preventDefault();

        const email = container.querySelector('#email').value;
        const password = container.querySelector('#password').value;

        // Disable button and show loading state
        submitButton.disabled = true;
        submitButton.innerHTML = `<span class="animate-spin">⏳</span> Signing In...`;

        // The login function from auth.js handles successful navigation
        const result = await login(email, password);

        // If login fails, show an error message
        if (!result.success) {
            showNotification(result.message, true);
            // Re-enable the button
            submitButton.disabled = false;
            submitButton.innerHTML = 'Sign In';
        }
    });

    // Handle clicks on the "Sign Up" button
    container.querySelector('#toggle-view-btn').addEventListener('click', () => {
        setAuthView('register');
    });
}
