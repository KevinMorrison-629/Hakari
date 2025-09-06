import { registerAndLogin } from '../auth.js';
import { setAuthView } from '../main.js';
import { showNotification } from '../ui/notification.js';

/**
 * Renders the registration page into a given container element.
 * @param {HTMLElement} container - The element to render the page in.
 */
export function renderRegisterView(container) {
    container.innerHTML = `
        <div class="container-centered">
            <div class="card">
                <div class="card-header">
                    <h2>Create Account</h2>
                    <p>Get started with your new account.</p>
                </div>
                <form id="registerForm">
                    <div class="form-group">
                        <label for="displayName" class="form-label">Display Name</label>
                        <input type="text" id="displayName" class="form-input" placeholder="Your Name" required>
                    </div>
                    <div class="form-group">
                        <label for="email" class="form-label">Email</label>
                        <input type="email" id="email" class="form-input" placeholder="you@example.com" required>
                    </div>
                    <div class="form-group">
                        <label for="password" class="form-label">Password</label>
                        <input type="password" id="password" class="form-input" placeholder="Choose a strong password" required>
                    </div>
                    <button type="submit" class="btn btn-primary">Create Account</button>
                    <button type="button" id="toggle-view-btn" class="btn btn-link">
                        Already have an account? Sign In
                    </button>
                </form>
            </div>
        </div>
    `;

    // --- Event Listeners ---
    const registerForm = container.querySelector('#registerForm');
    const submitButton = container.querySelector('button[type="submit"]');

    // Handle form submission
    registerForm.addEventListener('submit', async (event) => {
        event.preventDefault();

        const displayName = container.querySelector('#displayName').value;
        const email = container.querySelector('#email').value;
        const password = container.querySelector('#password').value;

        // Disable button and show loading state
        submitButton.disabled = true;
        submitButton.innerHTML = `<span class="animate-spin">⚙️</span> Creating Account...`;

        // Use the new function that handles both steps
        const result = await registerAndLogin(displayName, email, password);

        // If the process fails (either at registration or login), show an error.
        if (!result.success) {
            showNotification(result.message, true);
            submitButton.disabled = false;
            submitButton.innerHTML = 'Create Account';
        }
        // If successful, the `login` function within `registerAndLogin` will automatically
        // navigate to the main app view, so we don't need an 'else' block.
    });

    // Handle clicks on the "Sign In" button
    container.querySelector('#toggle-view-btn').addEventListener('click', () => {
        setAuthView('login');
    });
}
