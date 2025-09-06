import { getToken, logout } from '../auth.js';

/**
 * A wrapper around the native fetch API that automatically adds the
 * Authorization header and handles 401 Unauthorized responses by logging the user out.
 * @param {string} endpoint - The API endpoint to call (e.g., '/api/inventory').
 * @param {object} options - Standard fetch options (method, body, etc.).
 * @returns {Promise<Response>} The fetch Response object.
 */
export async function apiFetch(endpoint, options = {}) {
    const token = getToken();
    const headers = {
        'Content-Type': 'application/json',
        ...options.headers,
    };

    if (token) {
        headers['Authorization'] = `Bearer ${token}`;
    }

    const response = await fetch(endpoint, { ...options, headers });

    // If the server returns 401, the token is invalid or expired.
    // Log the user out and redirect them to the login page.
    if (response.status === 401) {
        logout();
        // Throw an error to stop the current execution chain.
        throw new Error('Unauthorized');
    }

    return response;
}
