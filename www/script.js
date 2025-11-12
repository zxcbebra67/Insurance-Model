// Global variables
let simulationInterval = null;
let editingInsuranceId = null;

// DOM elements
const capitalValueEl = document.getElementById('capital-value');
const insuranceCardsContainer = document.getElementById('insurance-cards-container');
const totalCustomersEl = document.getElementById('total-customers');
const currentMonthEl = document.getElementById('current-month');
const totalProductsEl = document.getElementById('total-products');
const simulationLogEl = document.getElementById('simulation-log');

// Initialize application
document.addEventListener('DOMContentLoaded', function() {
    console.log('🚀 Insurance app initialized');
    initTabs();
    initModals();
    
    // Check server connection before loading state
    checkServerConnection().then(connected => {
        if (connected) {
            loadState();
        }
    });
    
    // Event listeners
    document.getElementById('start-simulation-btn').addEventListener('click', startSimulation);
    document.getElementById('stop-simulation-btn').addEventListener('click', stopSimulation);
    document.getElementById('reset-simulation-btn').addEventListener('click', resetSimulation);
    document.getElementById('simulate-month-btn').addEventListener('click', simulateMonth);
    document.getElementById('change-capital-btn').addEventListener('click', showCapitalModal);
    document.getElementById('change-probability-btn').addEventListener('click', showProbabilityModal); // Новая кнопка
    document.getElementById('change-order-btn').addEventListener('click', showOrderModal);
});

// Check server connection
async function checkServerConnection() {
    try {
        const response = await fetch('http://localhost:8080/test');
        if (response.ok) {
            const result = await response.json();
            console.log('✅ Server is running:', result);
            addLog('Connected to server successfully');
            return true;
        } else {
            throw new Error(`HTTP ${response.status}`);
        }
    } catch (error) {
        console.error('❌ Cannot connect to server:', error);
        addLog('Error: Cannot connect to server. Make sure the C++ server is running on localhost:8080');
        return false;
    }
}

// Tab management
function initTabs() {
    const tabs = document.querySelectorAll('.tab');
    const tabContents = document.querySelectorAll('.tab-content');
    
    tabs.forEach(tab => {
        tab.addEventListener('click', function() {
            const tabId = this.getAttribute('data-tab');
            
            tabs.forEach(t => t.classList.remove('active'));
            tabContents.forEach(tc => tc.classList.remove('active'));
            
            this.classList.add('active');
            document.getElementById(`${tabId}-tab`).classList.add('active');
        });
    });
}
// Modal management
function initModals() {
    const modals = document.querySelectorAll('.modal');
    const closeButtons = document.querySelectorAll('.close-modal');
    
    // Insurance modal
    document.getElementById('add-insurance-btn').addEventListener('click', () => {
        document.getElementById('insurance-modal').style.display = 'flex';
        document.getElementById('modal-title').textContent = 'Add Insurance Product';
        document.getElementById('insurance-form').reset();
        editingInsuranceId = null;
    });
    
    // Close modals
    closeButtons.forEach(button => {
        button.addEventListener('click', function() {
            modals.forEach(modal => modal.style.display = 'none');
            editingInsuranceId = null;
        });
    });
    
    // Close on outside click
    window.addEventListener('click', function(event) {
        modals.forEach(modal => {
            if (event.target === modal) {
                modal.style.display = 'none';
                editingInsuranceId = null;
            }
        });
    });
    
    // Form submissions
    document.getElementById('insurance-form').addEventListener('submit', function(e) {
        e.preventDefault();
        if (editingInsuranceId !== null) {
            editInsurance();
        } else {
            addInsurance();
        }
    });
    
    document.getElementById('capital-form').addEventListener('submit', function(e) {
        e.preventDefault();
        setCapital();
    });
    
    // Новая форма для вероятности
    document.getElementById('probability-form').addEventListener('submit', function(e) {
        e.preventDefault();
        setProbability();
    });

    document.getElementById('order-form').addEventListener('submit', function(e) {
        e.preventDefault();
        setOrder();
    });
}

function showCapitalModal() {
    document.getElementById('capital-modal').style.display = 'flex';
    document.getElementById('new-capital').value = document.getElementById('capital-value').textContent.replace(/,/g, '');
}

// НОВАЯ ФУНКЦИЯ: Показать модальное окно вероятности
function showProbabilityModal() {
    document.getElementById('probability-modal').style.display = 'flex';
    // Устанавливаем текущее значение вероятности из состояния (если доступно)
    const currentProbability = document.getElementById('probability-value')?.textContent || '5.0';
    document.getElementById('new-probability').value = parseFloat(currentProbability);
}

function showOrderModal() {
    document.getElementById('order-modal').style.display = 'flex';
    const currentOrder = document.getElementById('order-value')?.textContent || '100';
    document.getElementById('new-order').value = parseInt(currentOrder);
}

// API functions
async function apiCall(endpoint, data = null) {
    try {
        const url = `http://localhost:8080${endpoint}`;
        console.log('📡 API call:', url);
        
        const options = {
            method: data ? 'POST' : 'GET',
        };
        
        if (data) {
            const params = new URLSearchParams();
            for (const key in data) {
                params.append(key, data[key]);
            }
            options.body = params.toString();
            options.headers = {
                'Content-Type': 'application/x-www-form-urlencoded',
            };
        }
        
        const response = await fetch(url, options);
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        return await response.json();
        
    } catch (error) {
        console.error('❌ API call failed:', error);
        addLog(`Error: ${error.message}`);
        return { status: 'error', message: error.message };
    }
}

async function loadState() {
    console.log('🔄 Loading state...');
    const state = await apiCall('/state');
    if (state && !state.error) {
        console.log('✅ State loaded:', state);
        updateDisplay(state);
    } else {
        console.error('❌ Failed to load state:', state);
        addLog('Failed to load application state');
    }
}

async function simulateMonth() {
    console.log('⏩ Simulating month...');
    const result = await apiCall('/simulate_month');
    if (result && result.status === 'success') {
        // Обновляем сообщение лога с информацией о вероятности
        const probabilityInfo = result.probability ? ` (probability: ${(result.probability * 100).toFixed(1)}%)` : '';
        addLog(`Month ${result.month} completed: +${formatNumber(result.income)} RUB income, -${formatNumber(result.losses)} RUB losses, +${result.new_customers} new customers${probabilityInfo}`);
        loadState();
    } else {
        const errorMsg = result?.message || 'Unknown error';
        addLog(`Error simulating month: ${errorMsg}`);
    }
}

async function addInsurance() {
    const form = document.getElementById('insurance-form');
    const formData = {
        type: form.querySelector('#insurance-type').value,
        fee: parseInt(form.querySelector('#insurance-fee').value),
        fee_period: parseInt(form.querySelector('#insurance-fee-period').value),
        duration: parseInt(form.querySelector('#insurance-duration').value),
        max_compensation: parseInt(form.querySelector('#insurance-max-compensation').value),
        franchise: parseInt(form.querySelector('#insurance-franchise').value)
    };
    
    console.log('➕ Adding insurance:', formData);
    const result = await apiCall('/add_insurance', formData);
    if (result && result.status === 'success') {
        addLog(`Added new ${formData.type} insurance product`);
        document.getElementById('insurance-modal').style.display = 'none';
        form.reset();
        loadState();
    } else {
        const errorMsg = result?.message || 'Unknown error';
        addLog(`Error adding insurance: ${errorMsg}`);
    }
}

async function editInsurance() {
    const form = document.getElementById('insurance-form');
    const formData = {
        id: editingInsuranceId,
        fee: parseInt(form.querySelector('#insurance-fee').value),
        fee_period: parseInt(form.querySelector('#insurance-fee-period').value),
        duration: parseInt(form.querySelector('#insurance-duration').value),
        max_compensation: parseInt(form.querySelector('#insurance-max-compensation').value),
        franchise: parseInt(form.querySelector('#insurance-franchise').value)
    };
    
    console.log('✏️ Editing insurance:', formData);
    const result = await apiCall('/edit_insurance', formData);
    if (result && result.status === 'success') {
        addLog(`Updated insurance product #${editingInsuranceId}`);
        document.getElementById('insurance-modal').style.display = 'none';
        form.reset();
        editingInsuranceId = null;
        loadState();
    } else {
        const errorMsg = result?.message || 'Unknown error';
        addLog(`Error editing insurance: ${errorMsg}`);
    }
}

async function deleteInsurance(id) {
    if (!confirm('Are you sure you want to delete this insurance product?')) {
        return;
    }
    
    console.log('🗑️ Deleting insurance:', id);
    const result = await apiCall('/delete_insurance', { id: id });
    if (result && result.status === 'success') {
        addLog(`Deleted insurance product #${id}`);
        loadState();
    } else {
        const errorMsg = result?.message || 'Unknown error';
        addLog(`Error deleting insurance: ${errorMsg}`);
    }
}

// НОВАЯ ФУНКЦИЯ: Установить вероятность
async function setProbability() {
    const newProbability = parseFloat(document.getElementById('new-probability').value);
    console.log('🎲 Setting probability to:', newProbability);
    
    if (newProbability < 0 || newProbability > 100) {
        addLog('Error: Probability must be between 0 and 100%');
        return;
    }
    
    // Конвертируем проценты в дробь (0.0 - 1.0)
    const probabilityFraction = newProbability / 100;
    
    const result = await apiCall('/set_probability', { probability: probabilityFraction });
    if (result && result.status === 'success') {
        addLog(`Probability set to ${newProbability}%`);
        document.getElementById('probability-modal').style.display = 'none';
        loadState();
    } else {
        const errorMsg = result?.message || 'Unknown error';
        addLog(`Error setting probability: ${errorMsg}`);
    }
}

async function setOrder() {
    const newOrder = parseInt(document.getElementById('new-order').value);
    console.log('👥 Setting new clients per month to:', newOrder);
    
    if (newOrder < 0 || newOrder > 10000) {
        addLog('Error: Number of new clients must be between 0 and 10000');
        return;
    }
    
    const result = await apiCall('/set_order', { order: newOrder });
    if (result && result.status === 'success') {
        addLog(`New clients per month set to ${newOrder}`);
        document.getElementById('order-modal').style.display = 'none';
        loadState();
    } else {
        const errorMsg = result?.message || 'Unknown error';
        addLog(`Error setting new clients: ${errorMsg}`);
    }
}

function showEditModal(insurance) {
    document.getElementById('insurance-modal').style.display = 'flex';
    document.getElementById('modal-title').textContent = 'Edit Insurance Product';
    
    // Заполняем форму данными страховки
    document.getElementById('insurance-type').value = insurance.type.toLowerCase().split(' ')[0];
    document.getElementById('insurance-fee').value = insurance.fee;
    document.getElementById('insurance-fee-period').value = insurance.fee_period;
    document.getElementById('insurance-duration').value = insurance.duration;
    document.getElementById('insurance-max-compensation').value = insurance.max_compensation;
    document.getElementById('insurance-franchise').value = insurance.franchise;
    
    // Блокируем изменение типа при редактировании
    document.getElementById('insurance-type').disabled = true;
    
    editingInsuranceId = insurance.id;
}

async function setCapital() {
    const newCapital = document.getElementById('new-capital').value;
    console.log('💰 Setting capital to:', newCapital);
    const result = await apiCall('/set_capital', { capital: newCapital });
    if (result && result.status === 'success') {
        addLog(`Capital set to ${formatNumber(newCapital)} RUB`);
        document.getElementById('capital-modal').style.display = 'none';
        loadState();
    } else {
        const errorMsg = result?.message || 'Unknown error';
        addLog(`Error setting capital: ${errorMsg}`);
    }
}

async function resetSimulation() {
    console.log('🔄 Resetting simulation...');
    const result = await apiCall('/reset');
    if (result && result.status === 'success') {
        addLog('Simulation reset to initial state');
        loadState();
    } else {
        const errorMsg = result?.message || 'Unknown error';
        addLog(`Error resetting simulation: ${errorMsg}`);
    }
}

// Display functions
function updateDisplay(state) {
    console.log('🎨 Updating display with state:', state);
    
    if (!state) {
        console.error('No state provided to updateDisplay');
        return;
    }
    
    // Update capital
    capitalValueEl.textContent = formatNumber(state.capital);
    
    // Update probability display if element exists
    if (state.probability !== undefined) {
        const probabilityValueEl = document.getElementById('probability-value');
        if (probabilityValueEl) {
            probabilityValueEl.textContent = (state.probability * 100).toFixed(1);
        }
    }
    
    // Update order display if element exists
    if (state.order !== undefined) {
        const orderValueEl = document.getElementById('order-value');
        if (orderValueEl) {
            orderValueEl.textContent = state.order;
        }
    }
    
    // Update stats
    currentMonthEl.textContent = state.current_month || 0;
    
    // Calculate total customers and products
    let totalCustomers = 0;
    if (state.insurance_products && Array.isArray(state.insurance_products)) {
        state.insurance_products.forEach(product => {
            totalCustomers += product.customers || 0;
        });
        totalProductsEl.textContent = state.insurance_products.length;
    } else {
        totalProductsEl.textContent = '0';
    }
    totalCustomersEl.textContent = totalCustomers;
    
    // Update insurance cards
    updateInsuranceCards(state.insurance_products || []);
}

function updateInsuranceCards(products) {
    console.log('🃏 Updating insurance cards:', products.length, 'products');
    
    insuranceCardsContainer.innerHTML = '';
    
    if (products.length === 0) {
        insuranceCardsContainer.innerHTML = '<div class="no-products">No insurance products available</div>';
        return;
    }
    
    products.forEach(product => {
        const demandPercentage = product.demand;
        const typeName = product.type || 'Unknown';
        
        const card = document.createElement('div');
        card.className = 'card';
        card.innerHTML = `
            <div class="card-header">
                <div class="card-title">${typeName}</div>
                <div class="card-type">${typeName.split(' ')[0]}</div>
            </div>
            <div class="card-body">
                <div class="card-item">
                    <span class="card-label">Fee:</span>
                    <span class="card-value">${formatNumber(product.fee || 0)} RUB</span>
                </div>
                <div class="card-item">
                    <span class="card-label">Fee Period:</span>
                    <span class="card-value">${product.fee_period || 0} months</span>
                </div>
                <div class="card-item">
                    <span class="card-label">Duration:</span>
                    <span class="card-value">${product.duration || 0} months</span>
                </div>
                <div class="card-item">
                    <span class="card-label">Max Compensation:</span>
                    <span class="card-value">${formatNumber(product.max_compensation || 0)} RUB</span>
                </div>
                <div class="card-item">
                    <span class="card-label">Franchise:</span>
                    <span class="card-value">${formatNumber(product.franchise || 0)} RUB</span>
                </div>
                <div class="card-item">
                    <span class="card-label">Customers:</span>
                    <span class="card-value">${product.customers || 0}</span>
                </div>
                <div class="card-item">
                    <span class="card-label">Demand:</span>
                    <span class="card-value">${(product.demand || 0).toFixed(2)}</span>
                </div>
                <div class="demand-indicator">
                    <div class="demand-fill" style="width: ${demandPercentage}%"></div>
                </div>
                <div class="card-actions">
                    <button class="btn-edit" onclick="showEditModal(${JSON.stringify(product).replace(/"/g, '&quot;')})">Edit</button>
                    <button class="btn-delete" onclick="deleteInsurance(${product.id})">Delete</button>
                </div>
            </div>
        `;
        
        insuranceCardsContainer.appendChild(card);
    });
}

// Simulation control
function startSimulation() {
    if (simulationInterval) {
        clearInterval(simulationInterval);
    }
    
    simulationInterval = setInterval(async () => {
        await simulateMonth();
    }, 1500);
    
    document.getElementById('start-simulation-btn').disabled = true;
    document.getElementById('stop-simulation-btn').disabled = false;
    addLog('Simulation started (1 month per 1.5 seconds)');
}

function stopSimulation() {
    if (simulationInterval) {
        clearInterval(simulationInterval);
        simulationInterval = null;
    }
    document.getElementById('start-simulation-btn').disabled = false;
    document.getElementById('stop-simulation-btn').disabled = true;
    addLog('Simulation stopped');
}

// Utility functions
function formatNumber(num) {
    return new Intl.NumberFormat('ru-RU').format(num);
}

function addLog(message) {
    const logEntry = document.createElement('div');
    logEntry.textContent = `[${new Date().toLocaleTimeString()}] ${message}`;
    simulationLogEl.appendChild(logEntry);
    simulationLogEl.scrollTop = simulationLogEl.scrollHeight;
}