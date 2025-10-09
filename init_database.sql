-- PostgreSQL Query Client Demo Database
-- Initial schema and sample data for streaming session

-- Drop existing tables if they exist
DROP TABLE IF EXISTS order_items;
DROP TABLE IF EXISTS orders;
DROP TABLE IF EXISTS products;
DROP TABLE IF EXISTS customers;
DROP TABLE IF EXISTS categories;

-- Create categories table
CREATE TABLE categories (
    category_id SERIAL PRIMARY KEY,
    category_name VARCHAR(50) NOT NULL,
    description TEXT
);

-- Create products table
CREATE TABLE products (
    product_id SERIAL PRIMARY KEY,
    product_name VARCHAR(100) NOT NULL,
    category_id INTEGER REFERENCES categories(category_id),
    price DECIMAL(10, 2) NOT NULL,
    stock_quantity INTEGER DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create customers table
CREATE TABLE customers (
    customer_id SERIAL PRIMARY KEY,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    phone VARCHAR(20),
    city VARCHAR(50),
    country VARCHAR(50),
    registered_date DATE DEFAULT CURRENT_DATE
);

-- Create orders table
CREATE TABLE orders (
    order_id SERIAL PRIMARY KEY,
    customer_id INTEGER REFERENCES customers(customer_id),
    order_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    total_amount DECIMAL(10, 2),
    status VARCHAR(20) DEFAULT 'pending'
);

-- Create order_items table (many-to-many relationship)
CREATE TABLE order_items (
    order_item_id SERIAL PRIMARY KEY,
    order_id INTEGER REFERENCES orders(order_id),
    product_id INTEGER REFERENCES products(product_id),
    quantity INTEGER NOT NULL,
    unit_price DECIMAL(10, 2) NOT NULL
);

-- Insert sample categories
INSERT INTO categories (category_name, description) VALUES
    ('Electronics', 'Electronic devices and accessories'),
    ('Books', 'Physical and digital books'),
    ('Clothing', 'Apparel and fashion items'),
    ('Home & Garden', 'Home improvement and garden supplies'),
    ('Sports', 'Sports equipment and outdoor gear');

-- Insert sample products
INSERT INTO products (product_name, category_id, price, stock_quantity) VALUES
    ('Laptop Pro 15"', 1, 1299.99, 25),
    ('Wireless Mouse', 1, 29.99, 150),
    ('USB-C Cable', 1, 12.99, 200),
    ('The Art of Programming', 2, 45.50, 60),
    ('Database Design Guide', 2, 39.99, 40),
    ('Cotton T-Shirt', 3, 19.99, 300),
    ('Denim Jeans', 3, 59.99, 120),
    ('Garden Hose 50ft', 4, 34.99, 45),
    ('LED Desk Lamp', 4, 42.00, 80),
    ('Basketball', 5, 24.99, 95),
    ('Yoga Mat', 5, 28.50, 110),
    ('Camping Tent 4-Person', 5, 189.99, 15);

-- Insert sample customers
INSERT INTO customers (first_name, last_name, email, phone, city, country) VALUES
    ('Alice', 'Johnson', 'alice.j@email.com', '555-0101', 'New York', 'USA'),
    ('Bob', 'Smith', 'bob.smith@email.com', '555-0102', 'London', 'UK'),
    ('Carol', 'Williams', 'carol.w@email.com', '555-0103', 'Toronto', 'Canada'),
    ('David', 'Brown', 'david.b@email.com', '555-0104', 'Sydney', 'Australia'),
    ('Emma', 'Davis', 'emma.davis@email.com', '555-0105', 'Berlin', 'Germany'),
    ('Frank', 'Miller', 'frank.m@email.com', '555-0106', 'Paris', 'France'),
    ('Grace', 'Wilson', 'grace.w@email.com', '555-0107', 'Tokyo', 'Japan'),
    ('Henry', 'Moore', 'henry.m@email.com', '555-0108', 'New York', 'USA'),
    ('Iris', 'Taylor', 'iris.t@email.com', '555-0109', 'London', 'UK'),
    ('Jack', 'Anderson', 'jack.a@email.com', '555-0110', 'Los Angeles', 'USA');

-- Insert sample orders
INSERT INTO orders (customer_id, order_date, total_amount, status) VALUES
    (1, '2024-01-15 10:30:00', 1329.98, 'completed'),
    (2, '2024-01-16 14:20:00', 45.50, 'completed'),
    (3, '2024-01-17 09:15:00', 79.98, 'completed'),
    (1, '2024-01-18 16:45:00', 42.00, 'shipped'),
    (4, '2024-01-19 11:00:00', 214.98, 'pending'),
    (5, '2024-01-20 13:30:00', 59.99, 'completed'),
    (6, '2024-01-21 15:10:00', 12.99, 'shipped'),
    (7, '2024-01-22 10:05:00', 189.99, 'pending'),
    (8, '2024-01-23 12:40:00', 85.49, 'completed'),
    (9, '2024-01-24 14:55:00', 29.99, 'completed');

-- Insert sample order items
INSERT INTO order_items (order_id, product_id, quantity, unit_price) VALUES
    -- Order 1 (Alice): Laptop + Mouse
    (1, 1, 1, 1299.99),
    (1, 2, 1, 29.99),
    -- Order 2 (Bob): Book
    (2, 4, 1, 45.50),
    -- Order 3 (Carol): T-Shirt and Jeans
    (3, 6, 1, 19.99),
    (3, 7, 1, 59.99),
    -- Order 4 (Alice): Desk Lamp
    (4, 9, 1, 42.00),
    -- Order 5 (David): Camping Tent + Basketball
    (5, 12, 1, 189.99),
    (5, 10, 1, 24.99),
    -- Order 6 (Emma): Denim Jeans
    (6, 7, 1, 59.99),
    -- Order 7 (Frank): USB-C Cable
    (7, 3, 1, 12.99),
    -- Order 8 (Grace): Camping Tent
    (8, 12, 1, 189.99),
    -- Order 9 (Henry): Database Book + Books on Programming
    (9, 5, 1, 39.99),
    (9, 4, 1, 45.50),
    -- Order 10 (Iris): Wireless Mouse
    (10, 2, 1, 29.99);

-- Create some useful views for demo queries
CREATE VIEW customer_order_summary AS
SELECT
    c.customer_id,
    c.first_name,
    c.last_name,
    c.email,
    COUNT(o.order_id) as total_orders,
    COALESCE(SUM(o.total_amount), 0) as total_spent
FROM customers c
LEFT JOIN orders o ON c.customer_id = o.customer_id
GROUP BY c.customer_id, c.first_name, c.last_name, c.email;

CREATE VIEW product_sales AS
SELECT
    p.product_id,
    p.product_name,
    c.category_name,
    p.price,
    p.stock_quantity,
    COALESCE(SUM(oi.quantity), 0) as total_sold
FROM products p
LEFT JOIN categories c ON p.category_id = c.category_id
LEFT JOIN order_items oi ON p.product_id = oi.product_id
GROUP BY p.product_id, p.product_name, c.category_name, p.price, p.stock_quantity;

-- Display summary
SELECT 'Database initialized successfully!' as message;
SELECT 'Total Categories: ' || COUNT(*) as info FROM categories;
SELECT 'Total Products: ' || COUNT(*) as info FROM products;
SELECT 'Total Customers: ' || COUNT(*) as info FROM customers;
SELECT 'Total Orders: ' || COUNT(*) as info FROM orders;
