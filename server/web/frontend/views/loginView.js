import { login, register } from '../auth.js';

let isLoginView = true;
let isLoading = false;
let message = { text: '', isError: false };

/**
 * Renders the entire login/registration view and attaches event listeners.
 * @param {HTMLElement} container - The element to render the view into.
 */
export function renderLoginView(container) {
    // Reset state on each render
    isLoading = false;
    message = { text: '', isError: false };

    container.innerHTML = `
        <div class="container-centered">
            <div class="card">
                <div class="card-header">
                    <h2 id="form-title">Welcome Back</h2>
                    <p id="form-subtitle">Sign in to continue to CardForge</p>
                </div>
                <form id="auth-form">
                    <div id="message-container"></div>
                    <div class="form-group">
                        <label for="email" class="form-label">Email</label>
                        <input type="email" id="email" required class="form-input" />
                    </div>
                    <div class="form-group">
                        <label for="password" class="form-label">Password</label>
                        <input type="password" id="password" required class="form-input" />
                    </div>
                    <div style="display: flex; flex-direction: column; gap: 1rem;">
                        <button type="submit" id="submit-btn" class="btn btn-primary">
                            Sign In
                        </button>
                        <button type="button" id="toggle-view-btn" class="btn btn-link">
                            Don't have an account? Sign up
                        </button>
                    </div>
                </form>
            </div>
        </div>
    `;

    // Add event listeners after the DOM is updated
    attachEventListeners();
}

/**
 * Updates the view's dynamic content (titles, messages, buttons).
 */
function updateView() {
    const formTitle = document.getElementById('form-title');
    const formSubtitle = document.getElementById('form-subtitle');
    const submitBtn = document.getElementById('submit-btn');
    const toggleBtn = document.getElementById('toggle-view-btn');
    const messageContainer = document.getElementById('message-container');

    // Update titles and button text
    if (isLoginView) {
        formTitle.textContent = 'Welcome Back';
        formSubtitle.textContent = 'Sign in to continue to CardForge';
        submitBtn.innerHTML = isLoading ? `<span class="animate-spin">⏳</span> Processing...` : 'Sign In';
        toggleBtn.textContent = "Don't have an account? Sign up";
    } else {
        formTitle.textContent = 'Create Account';
        formSubtitle.textContent = 'Get started with a new account';
        submitBtn.innerHTML = isLoading ? `<span class="animate-spin">⏳</span> Processing...` : 'Create Account';
        toggleBtn.textContent = 'Already have an account? Sign in';
    }

    // Update loading state
    submitBtn.disabled = isLoading;

    // Display message if it exists
    messageContainer.innerHTML = '';
    if (message.text) {
        const messageClass = message.isError ? 'message-error' : 'message-success';
        messageContainer.innerHTML = `<div class="message-box ${messageClass}">${message.text}</div>`;
    }
}

/**
 * Attaches event listeners to the form and buttons.
 */
function attachEventListeners() {
    const authForm = document.getElementById('auth-form');
    const toggleBtn = document.getElementById('toggle-view-btn');

    authForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        isLoading = true;
        message = { text: '', isError: false };
        updateView();

        const email = document.getElementById('email').value;
        const password = document.getElementById('password').value;

        let result;
        if (isLoginView) {
            result = await login(email, password);
        } else {
            result = await register(email, password);
        }

        isLoading = false;

        if (result.success) {
            // Login function handles navigation, for registration we show a message and switch views
            if (!isLoginView) {
                isLoginView = true;
                message = { text: result.message, isError: false };
                updateView();
                document.getElementById('password').value = ''; // Clear password field
            }
        } else {
            message = { text: result.message, isError: true };
            updateView();
        }
    });

    toggleBtn.addEventListener('click', () => {
        isLoginView = !isLoginView;
        message = { text: '', isError: false }; // Clear message on view toggle
        updateView();
    });
}
