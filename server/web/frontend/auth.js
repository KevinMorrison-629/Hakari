import { navigate } from './main.js';
import { apiFetch } from './api.js';

const AUTH_TOKEN_KEY = 'authToken';

/**
 * Checks if a token exists in localStorage.
 * @returns {boolean} True if the user is authenticated, false otherwise.
 */
export function isAuthenticated() {
    return !!localStorage.getItem(AUTH_TOKEN_KEY);
}

/**
 * Saves the authentication token to localStorage.
 * @param {string} token - The JWT or session token.
 */
function setToken(token) {
    if (token) {
        localStorage.setItem(AUTH_TOKEN_KEY, token);
    } else {
        localStorage.removeItem(AUTH_TOKEN_KEY);
    }
}

/**
 * Retrieves the authentication token from localStorage.
 * @returns {string|null} The token, or null if it doesn't exist.
 */
export function getToken() {
    return localStorage.getItem(AUTH_TOKEN_KEY);
}

/**
 * Handles the user login process.
 * @param {string} email
 * @param {string} password
 * @returns {Promise<{success: boolean, message: string}>}
 */
export async function login(email, password) {
    try {
        const response = await apiFetch('/api/login', {
            method: 'POST',
            body: JSON.stringify({ email, password }),
        });
        const data = await response.json();

        if (data.success && data.token) {
            setToken(data.token);
            navigate(); // Re-render the app to show the main view
            return { success: true };
        }
        return { success: false, message: data.message || 'Login failed.' };
    } catch (error) {
        console.error('Login error:', error);
        return { success: false, message: 'Could not connect to the server.' };
    }
}

/**
 * Handles the user registration process.
 * @param {string} email
 * @param {string} password
 * @returns {Promise<{success: boolean, message: string}>}
 */
export async function register(email, password) {
    try {
        const response = await fetch('/api/register', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ email, password }),
        });
        const data = await response.json();
        return { success: data.success, message: data.message || 'An error occurred.' };
    } catch (error) {
        console.error('Registration error:', error);
        return { success: false, message: 'Could not connect to the server.' };
    }
}


/**
 * Logs the user out by removing the token and re-navigating.
 */
export function logout() {
    setToken(null);
    navigate();
}

