import React, { useState, createContext, useContext } from 'react';

// --- Authentication Context ---
// This will hold the user state and be accessible throughout the app.
const AuthContext = createContext(null);

const AuthProvider = ({ children }) => {
  const [user, setUser] = useState(null);
  const [error, setError] = useState(null);

  const login = async (email, password) => {
    setError(null);
    try {
      const response = await fetch('/api/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ email, password }),
      });
      const data = await response.json();
      if (data.success) {
        setUser({ email: data.user.email, username: data.user.username, token: data.token });
        return true;
      } else {
        setError(data.message || 'Login failed.');
        return false;
      }
    } catch (err) {
      setError('Could not connect to the server.');
      return false;
    }
  };

  const register = async (username, email, password) => {
    setError(null);
    try {
      const response = await fetch('/api/register', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username, email, password }),
      });
      const data = await response.json();
      if (data.success) {
        // Automatically log in after successful registration
        setUser({ email: data.user.email, username: data.user.username, token: data.token });
        return true;
      } else {
        setError(data.message || 'Registration failed.');
        return false;
      }
    } catch (err) {
      setError('Could not connect to the server.');
      return false;
    }
  };


  const logout = () => {
    setUser(null);
  };

  return (
    <AuthContext.Provider value={{ user, login, register, logout, error, setError }}>
      {children}
    </AuthContext.Provider>
  );
};

// Custom hook to use the auth context
const useAuth = () => useContext(AuthContext);

// --- Main App Component ---
function App() {
  return (
    <AuthProvider>
      <div className="bg-gray-900 text-white min-h-screen flex flex-col items-center justify-center font-sans p-4">
        <div className="w-full max-w-md">
          <h1 className="text-5xl font-bold text-center mb-2 text-purple-400 tracking-wider">CardForge</h1>
          <p className="text-center text-gray-400 mb-8">Your digital card collection awaits.</p>
          <MainContent />
        </div>
      </div>
    </AuthProvider>
  );
}

// --- Content switching ---
const MainContent = () => {
  const { user } = useAuth();
  // If user is logged in, show the game page, otherwise show the login/register page
  return user ? <GamePage /> : <LoginPage />;
}


// --- Placeholder Game Page ---
const GamePage = () => {
  const { user, logout } = useAuth();
  return (
    <div className="bg-gray-800 p-8 rounded-lg shadow-2xl w-full text-center">
      <h2 className="text-2xl font-bold mb-4">Welcome, {user.username}!</h2>
      <p className="text-gray-300 mb-6">You are logged in as {user.email}.</p>
      <button
        onClick={logout}
        className="w-full bg-red-600 hover:bg-red-700 text-white font-bold py-3 px-4 rounded-lg transition duration-300"
      >
        Logout
      </button>
    </div>
  );
};


// --- Login Page Component ---
const LoginPage = () => {
  const [isCreatingAccount, setIsCreatingAccount] = useState(false);
  const { error, setError } = useAuth();

  const toggleView = () => {
    setIsCreatingAccount(!isCreatingAccount);
    setError(null); // Clear errors when switching views
  };

  return (
    <div className="bg-gray-800 p-8 rounded-lg shadow-2xl w-full">
      {isCreatingAccount ? (
        <CreateAccountComponent />
      ) : (
        <LoginComponent />
      )}

      {error && <ErrorDisplay message={error} />}

      <div className="mt-6 text-center">
        <button onClick={toggleView} className="text-purple-400 hover:text-purple-300 text-sm font-semibold">
          {isCreatingAccount ? "Already have an account? Sign In" : "Need an account? Create one"}
        </button>
      </div>
    </div>
  );
};

// --- Error Display ---
const ErrorDisplay = ({ message }) => (
  <div className="bg-red-900 border border-red-700 text-red-200 px-4 py-3 rounded-lg mt-4 text-center text-sm" role="alert">
    <p>{message}</p>
  </div>
);


// --- Login Form ---
const LoginComponent = () => {
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [isLoading, setIsLoading] = useState(false);
  const { login } = useAuth();

  const handleLogin = async (e) => {
    e.preventDefault();
    setIsLoading(true);
    await login(email, password);
    setIsLoading(false);
  };

  return (
    <>
      <h2 className="text-2xl font-bold text-center mb-6">Sign In</h2>
      <form onSubmit={handleLogin}>
        {/* Email and Password fields... */}
        <div className="mb-4">
          <label className="block text-gray-400 text-sm font-bold mb-2" htmlFor="email">Email</label>
          <input type="email" id="email" value={email} onChange={e => setEmail(e.target.value)} className="shadow appearance-none border border-gray-700 rounded-lg w-full py-3 px-4 bg-gray-700 text-white leading-tight focus:outline-none focus:shadow-outline focus:border-purple-500" required />
        </div>
        <div className="mb-6">
          <label className="block text-gray-400 text-sm font-bold mb-2" htmlFor="password">Password</label>
          <input type="password" id="password" value={password} onChange={e => setPassword(e.target.value)} className="shadow appearance-none border border-gray-700 rounded-lg w-full py-3 px-4 bg-gray-700 text-white leading-tight focus:outline-none focus:shadow-outline focus:border-purple-500" required />
        </div>
        <button type="submit" disabled={isLoading} className="w-full bg-purple-600 hover:bg-purple-700 text-white font-bold py-3 px-4 rounded-lg transition duration-300 disabled:bg-gray-600">
          {isLoading ? 'Signing In...' : 'Sign In'}
        </button>
      </form>
    </>
  );
};

// --- Create Account Form ---
const CreateAccountComponent = () => {
  const [username, setUsername] = useState('');
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [isLoading, setIsLoading] = useState(false);
  const { register } = useAuth();

  const handleCreateAccount = async (e) => {
    e.preventDefault();
    setIsLoading(true);
    await register(username, email, password);
    setIsLoading(false);
  };

  return (
    <>
      <h2 className="text-2xl font-bold text-center mb-6">Create Account</h2>
      <form onSubmit={handleCreateAccount}>
        {/* Username, Email, and Password fields... */}
        <div className="mb-4">
          <label className="block text-gray-400 text-sm font-bold mb-2" htmlFor="new-username">Username</label>
          <input type="text" id="new-username" value={username} onChange={e => setUsername(e.target.value)} className="shadow appearance-none border border-gray-700 rounded-lg w-full py-3 px-4 bg-gray-700 text-white leading-tight focus:outline-none focus:shadow-outline focus:border-purple-500" required />
        </div>
        <div className="mb-4">
          <label className="block text-gray-400 text-sm font-bold mb-2" htmlFor="new-email">Email</label>
          <input type="email" id="new-email" value={email} onChange={e => setEmail(e.target.value)} className="shadow appearance-none border border-gray-700 rounded-lg w-full py-3 px-4 bg-gray-700 text-white leading-tight focus:outline-none focus:shadow-outline focus:border-purple-500" required />
        </div>
        <div className="mb-6">
          <label className="block text-gray-400 text-sm font-bold mb-2" htmlFor="new-password">Password</label>
          <input type="password" id="new-password" value={password} onChange={e => setPassword(e.target.value)} className="shadow appearance-none border border-gray-700 rounded-lg w-full py-3 px-4 bg-gray-700 text-white leading-tight focus:outline-none focus:shadow-outline focus:border-purple-500" required />
        </div>
        <button type="submit" disabled={isLoading} className="w-full bg-green-600 hover:bg-green-700 text-white font-bold py-3 px-4 rounded-lg transition duration-300 disabled:bg-gray-600">
          {isLoading ? 'Creating...' : 'Create Account'}
        </button>
      </form>
    </>
  );
};

export default App;

